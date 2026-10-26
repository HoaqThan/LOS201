#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "asgn1_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Than Ngoc Tan Hoang - SE207003");
MODULE_DESCRIPTION("Assignment 1 - Driver with ioctl interface");
MODULE_VERSION("1.0");

#define DEVICE_NAME  "asgn1"
#define CLASS_NAME   "asgn1_class"
#define BUFFER_SIZE  1024
#define DRIVER_MAJOR 241

static int major_number = DRIVER_MAJOR;
static char device_buffer[BUFFER_SIZE];
static int buffer_len = 0;
static int open_count = 0;
static int write_count = 0;
static int read_count = 0;
static int last_write_size = 0;
static int operating_mode = 1; /* Mặc định 1: Echo mode */

static struct class  *asgn1_class  = NULL;
static struct device *asgn1_device = NULL;
static DEFINE_MUTEX(asgn1_mutex);

static int asgn1_open(struct inode *inode, struct file *file) {
    mutex_lock(&asgn1_mutex);
    open_count++;
    mutex_unlock(&asgn1_mutex);
    pr_info("asgn1_driver: device opened (count=%d)\n", open_count);
    return 0;
}

static int asgn1_release(struct inode *inode, struct file *file) {
    pr_info("asgn1_driver: device closed\n");
    return 0;
}

static ssize_t asgn1_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int bytes_read = 0;
    mutex_lock(&asgn1_mutex);
    if (*offset >= buffer_len) {
        mutex_unlock(&asgn1_mutex);
        return 0;
    }
    if (len > buffer_len - *offset)
        len = buffer_len - *offset;

    if (copy_to_user(buf, device_buffer + *offset, len)) {
        mutex_unlock(&asgn1_mutex);
        pr_err("asgn1_driver: copy_to_user failed\n");
        return -EFAULT;
    }
    *offset += len;
    bytes_read = len;
    read_count++;
    mutex_unlock(&asgn1_mutex);
    pr_info("asgn1_driver: read %d bytes\n", bytes_read);
    return bytes_read;
}

static ssize_t asgn1_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    mutex_lock(&asgn1_mutex);
    if (operating_mode == 0) { /* Kernel mode: Tự thêm tiền tố [KERNEL] */
        const char *prefix = "[KERNEL] ";
        size_t prefix_len = 9;
        if (len + prefix_len > BUFFER_SIZE - 1)
            len = BUFFER_SIZE - 1 - prefix_len;
        
        memcpy(device_buffer, prefix, prefix_len);
        if (copy_from_user(device_buffer + prefix_len, buf, len)) {
            mutex_unlock(&asgn1_mutex);
            return -EFAULT;
        }
        buffer_len = len + prefix_len;
    } else { /* Echo mode */
        if (len > BUFFER_SIZE - 1)
            len = BUFFER_SIZE - 1;
        if (copy_from_user(device_buffer, buf, len)) {
            mutex_unlock(&asgn1_mutex);
            return -EFAULT;
        }
        buffer_len = len;
    }
    device_buffer[buffer_len] = '\0';
    write_count++;
    last_write_size = len;
    mutex_unlock(&asgn1_mutex);
    pr_info("asgn1_driver: written %zu bytes\n", len);
    return len;
}

static long asgn1_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    mutex_lock(&asgn1_mutex);
    switch (cmd) {
        case ASGN1_RESET_BUFFER:
            memset(device_buffer, 0, BUFFER_SIZE);
            buffer_len = 0;
            write_count = 0;
            read_count = 0;
            pr_info("asgn1_driver: ioctl RESET_BUFFER executed\n");
            break;

        case ASGN1_GET_STATS: {
            struct asgn1_stats stats = {
                .open_count = open_count,
                .write_count = write_count,
                .read_count = read_count,
                .buffer_len = buffer_len,
                .last_write_size = last_write_size
            };
            if (copy_to_user((struct asgn1_stats __user *)arg, &stats, sizeof(stats))) {
                mutex_unlock(&asgn1_mutex);
                return -EFAULT;
            }
            pr_info("asgn1_driver: ioctl GET_STATS executed\n");
            break;
        }

        case ASGN1_SET_MODE: {
            int new_mode;
            if (copy_from_user(&new_mode, (int __user *)arg, sizeof(int))) {
                mutex_unlock(&asgn1_mutex);
                return -EFAULT;
            }
            if (new_mode != 0 && new_mode != 1) {
                mutex_unlock(&asgn1_mutex);
                return -EINVAL;
            }
            operating_mode = new_mode;
            pr_info("asgn1_driver: ioctl SET_MODE set to %d\n", operating_mode);
            break;
        }

        case ASGN1_GET_VERSION: {
            char ver[64] = "asgn1_driver v1.0 - SE207003";
            if (copy_to_user((char __user *)arg, ver, sizeof(ver))) {
                mutex_unlock(&asgn1_mutex);
                return -EFAULT;
            }
            pr_info("asgn1_driver: ioctl GET_VERSION executed\n");
            break;
        }

        default:
            mutex_unlock(&asgn1_mutex);
            return -ENOTTY;
    }
    mutex_unlock(&asgn1_mutex);
    return 0;
}

static struct file_operations asgn1_fops = {
    .owner          = THIS_MODULE,
    .open           = asgn1_open,
    .release        = asgn1_release,
    .read           = asgn1_read,
    .write          = asgn1_write,
    .unlocked_ioctl = asgn1_ioctl,
};

static int __init asgn1_init(void) {
    int ret = register_chrdev(major_number, DEVICE_NAME, &asgn1_fops);
    if (ret < 0) return ret;

    asgn1_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(asgn1_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(asgn1_class);
    }

    asgn1_device = device_create(asgn1_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(asgn1_device)) {
        class_destroy(asgn1_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(asgn1_device);
    }
    pr_info("asgn1_driver: loaded successfully with major=%d\n", major_number);
    return 0;
}

static void __exit asgn1_exit(void) {
    device_destroy(asgn1_class, MKDEV(major_number, 0));
    class_destroy(asgn1_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("asgn1_driver: unloaded\n");
}

module_init(asgn1_init);
module_exit(asgn1_exit);
