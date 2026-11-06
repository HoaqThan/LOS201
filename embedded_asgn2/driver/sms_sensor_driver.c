#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/random.h>
#include <linux/ktime.h>
#include <linux/mutex.h>

#define DRIVER_NAME "sms_sensor"
#define SMS_IOC_MAGIC 'S'
#define SMS_SET_INTERVAL _IOW(SMS_IOC_MAGIC, 1, unsigned int)
#define SMS_GET_INTERVAL _IOR(SMS_IOC_MAGIC, 2, unsigned int)
struct sensor_data {
    u64 timestamp_ms;
    int temperature;
    int humidity;
};

static DEFINE_MUTEX(sensor_lock);
static unsigned long sample_count;
static unsigned int sample_interval_ms = 500;

static void generate_sensor_data(struct sensor_data *data)
{
    u32 random_value;

   data->timestamp_ms = ktime_to_ms(ktime_get());

    get_random_bytes(&random_value, sizeof(random_value));
    data->temperature = 20 + (random_value % 31);

    get_random_bytes(&random_value, sizeof(random_value));
    data->humidity = 30 + (random_value % 61);
}

static ssize_t sms_read(struct file *file,
                        char __user *buf,
                        size_t count,
                        loff_t *ppos)
{
    struct sensor_data data;
    char output[128];
    int len;

    if (*ppos > 0)
        return 0;

    mutex_lock(&sensor_lock);

    generate_sensor_data(&data);
    sample_count++;

    mutex_unlock(&sensor_lock);

    len = scnprintf(output, sizeof(output),
                    "%llu,%d,%d\n",
                    (unsigned long long)data.timestamp_ms,
                    data.temperature,
                    data.humidity);

    return simple_read_from_buffer(buf, count, ppos,
                                   output, len);
}

static long sms_ioctl(struct file *file,
                      unsigned int cmd,
                      unsigned long arg)
{
    unsigned int value;

    switch (cmd) {
    case SMS_SET_INTERVAL:
        if (copy_from_user(&value,
                           (void __user *)arg,
                           sizeof(value)))
            return -EFAULT;

        if (value < 100 || value > 10000)
            return -EINVAL;

        mutex_lock(&sensor_lock);
        sample_interval_ms = value;
        mutex_unlock(&sensor_lock);
        return 0;

    case SMS_GET_INTERVAL:
        mutex_lock(&sensor_lock);
        value = sample_interval_ms;
        mutex_unlock(&sensor_lock);

        if (copy_to_user((void __user *)arg,
                         &value, sizeof(value)))
            return -EFAULT;
        return 0;

    default:
        return -ENOTTY;
    }
}

static int sms_proc_show(struct seq_file *m, void *v)
{
    unsigned long count;
    unsigned int interval;

    mutex_lock(&sensor_lock);
    count = sample_count;
    interval = sample_interval_ms;
    mutex_unlock(&sensor_lock);

    seq_printf(m, "SMS Sensor Statistics\n");
    seq_printf(m, "=====================\n");
    seq_printf(m, "Total samples: %lu\n", count);
    seq_printf(m, "Sample interval: %u ms\n", interval);

    return 0;
}

static int sms_proc_open(struct inode *inode,
                         struct file *file)
{
    return single_open(file, sms_proc_show, NULL);
}

static const struct proc_ops sms_proc_ops = {
    .proc_open    = sms_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static const struct file_operations sms_fops = {
    .owner          = THIS_MODULE,
    .read           = sms_read,
    .unlocked_ioctl = sms_ioctl,
    .llseek         = no_llseek,
};

static struct miscdevice sms_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DRIVER_NAME,
    .fops  = &sms_fops,
    .mode  = 0666,
};

static int __init sms_init(void)
{
    int ret;

    ret = misc_register(&sms_miscdev);
    if (ret) {
        pr_err("sms_sensor: device registration failed\n");
        return ret;
    }

    if (!proc_create("sms_stats", 0444, NULL,
                     &sms_proc_ops)) {
        misc_deregister(&sms_miscdev);
        return -ENOMEM;
    }

    pr_info("sms_sensor: driver loaded successfully\n");
    pr_info("sms_sensor: /dev/sms_sensor registered\n");
    pr_info("sms_sensor: /proc/sms_stats created\n");

    return 0;
}


static void __exit sms_exit(void)
{
    remove_proc_entry("sms_stats", NULL);
    misc_deregister(&sms_miscdev);

    pr_info("sms_sensor: driver unloaded\n");
}

module_init(sms_init);
module_exit(sms_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LOS201 Student");
MODULE_DESCRIPTION("SMS Virtual Sensor Driver");
MODULE_VERSION("1.0");

