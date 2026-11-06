
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#define DEVICE_PATH "/dev/sms_sensor"
#define LOG_PATH "/tmp/sms.log"
#define BUFFER_SIZE 32
#define SAMPLE_INTERVAL_MS 500

typedef struct {
    unsigned long long timestamp;
    int temperature;
    int humidity;
} SensorData;

static SensorData buffer[BUFFER_SIZE];
static int head = 0;
static int tail = 0;
static int count = 0;

static pthread_mutex_t buffer_mutex =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t buffer_not_empty =
    PTHREAD_COND_INITIALIZER;

static pthread_cond_t buffer_not_full =
    PTHREAD_COND_INITIALIZER;

static volatile int running = 1;

static pthread_mutex_t result_mutex =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t result_ready =
    PTHREAD_COND_INITIALIZER;

static SensorData latest_result;
static unsigned long result_version = 0;
static int latest_anomaly = 0;

static void buffer_push(SensorData data)
{
    pthread_mutex_lock(&buffer_mutex);

    while (count == BUFFER_SIZE)
        pthread_cond_wait(&buffer_not_full,
                          &buffer_mutex);

    buffer[head] = data;
    head = (head + 1) % BUFFER_SIZE;
    count++;

    pthread_cond_signal(&buffer_not_empty);
    pthread_mutex_unlock(&buffer_mutex);
}

static SensorData buffer_pop(void)
{
    SensorData data;

    pthread_mutex_lock(&buffer_mutex);

    while (count == 0 && running)
    pthread_cond_wait(&buffer_not_empty,
                      &buffer_mutex);

if (count == 0) {
    pthread_mutex_unlock(&buffer_mutex);
    SensorData empty = {0};
    return empty;
}

    data = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    count--;

    pthread_cond_signal(&buffer_not_full);
    pthread_mutex_unlock(&buffer_mutex);

    return data;
}

static void *sensor_thread(void *arg)
{
    int fd = -1;
    char line[128];
    ssize_t n;
    SensorData data;

    (void)arg;

    while (running) {
    fd = open(DEVICE_PATH, O_RDONLY);

    if (fd < 0) {
        perror("open sensor");
        break;
    }

    n = read(fd, line, sizeof(line) - 1);
    close(fd);

        if (n > 0) {
            line[n] = '\0';

            if (sscanf(line, "%llu,%d,%d",
                       &data.timestamp,
                       &data.temperature,
                       &data.humidity) == 3) {

                buffer_push(data);

                printf("[SENSOR] Temp=%d C, Hum=%d%%\n",
                       data.temperature,
                       data.humidity);
            }
        }

        usleep(SAMPLE_INTERVAL_MS * 1000);
    }

    close(fd);
    return NULL;
}

static void *processor_thread(void *arg)
{
    SensorData data;
    long total_temp = 0;
    long total_hum = 0;
    unsigned long processed = 0;
    double avg_temp, avg_hum;

    (void)arg;

    while (running) {
        data = buffer_pop();

	if (!running && data.timestamp == 0)
    break;

        processed++;
        total_temp += data.temperature;
        total_hum += data.humidity;

        avg_temp = (double)total_temp / processed;
        avg_hum = (double)total_hum / processed;

        printf("[PROCESSOR] Sample=%lu "
               "Avg Temp=%.2f C "
               "Avg Hum=%.2f%%\n",
               processed, avg_temp, avg_hum);

        
pthread_mutex_lock(&result_mutex);

latest_result = data;
latest_anomaly =
    (data.temperature > 40 || data.humidity > 80);

result_version++;

pthread_cond_broadcast(&result_ready);
pthread_mutex_unlock(&result_mutex);

        }


    return NULL;
}

static void *alert_thread(void *arg)
{
    unsigned long seen = 0;

    (void)arg;

    while (running) {
        SensorData data;
        int anomaly;

        pthread_mutex_lock(&result_mutex);

        while (seen == result_version && running)
            pthread_cond_wait(&result_ready, &result_mutex);

        if (!running) {
            pthread_mutex_unlock(&result_mutex);
            break;
        }

        seen = result_version;
        data = latest_result;
        anomaly = latest_anomaly;

        pthread_mutex_unlock(&result_mutex);

        if (anomaly) {
            printf("[ALERT] WARNING! Temp=%d C, "
                   "Humidity=%d%%\n",
                   data.temperature, data.humidity);
        }
    }

    return NULL;
}

static void *logger_thread(void *arg)
{
    FILE *fp;
    unsigned long seen = 0;

    (void)arg;

    fp = fopen(LOG_PATH, "a");
    if (!fp) {
        perror("open log");
        return NULL;
    }

    while (running) {
        SensorData data;

        pthread_mutex_lock(&result_mutex);

        while (seen == result_version && running)
            pthread_cond_wait(&result_ready, &result_mutex);

        if (!running) {
            pthread_mutex_unlock(&result_mutex);
            break;
        }

        seen = result_version;
        data = latest_result;

        pthread_mutex_unlock(&result_mutex);

        fprintf(fp, "%llu,%d,%d\n",
                data.timestamp,
                data.temperature,
                data.humidity);

        fflush(fp);
    }

    fclose(fp);
    return NULL;
}

int main(void)
{
    pthread_t sensor_tid;
    pthread_t processor_tid;
    pthread_t alert_tid;
    pthread_t logger_tid;

    printf("SMS Monitoring System Starting...\n");

    if (pthread_create(&sensor_tid, NULL,
                       sensor_thread, NULL) != 0) {
        fprintf(stderr, "Cannot create sensor thread\n");
        return 1;
    }

    if (pthread_create(&processor_tid, NULL,
                       processor_thread, NULL) != 0) {
        fprintf(stderr, "Cannot create processor thread\n");
        return 1;
    }

    if (pthread_create(&alert_tid, NULL,
                       alert_thread, NULL) != 0) {
        fprintf(stderr, "Cannot create alert thread\n");
        return 1;
    }

    if (pthread_create(&logger_tid, NULL,
                       logger_thread, NULL) != 0) {
        fprintf(stderr, "Cannot create logger thread\n");
        return 1;
    }

    sleep(20);

    pthread_mutex_lock(&buffer_mutex);
    running = 0;
    pthread_cond_broadcast(&buffer_not_empty);
    pthread_cond_broadcast(&buffer_not_full);
    pthread_mutex_unlock(&buffer_mutex);

    pthread_mutex_lock(&result_mutex);
    pthread_cond_broadcast(&result_ready);
    pthread_mutex_unlock(&result_mutex);

    pthread_join(sensor_tid, NULL);
    pthread_join(processor_tid, NULL);
    pthread_join(alert_tid, NULL);
    pthread_join(logger_tid, NULL);

    printf("SMS Monitoring System Stopped.\n");
    return 0;
}
