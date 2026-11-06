

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/atomic.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define DEVICE_NAME "counter"
#define PROC_NAME   "counter_info"

/* Atomic counters */
static atomic_t counter = ATOMIC_INIT(0);
static atomic_t read_count = ATOMIC_INIT(0);
static atomic_t write_count = ATOMIC_INIT(0);

/* Read current value, then increment */
static ssize_t counter_read(struct file *file,
                            char __user *buf,
                            size_t len,
                            loff_t *ppos)
{
    char temp[32];
    int value;
    int size;

    if (*ppos != 0)
        return 0;

    value = atomic_fetch_inc(&counter);
    size = scnprintf(temp, sizeof(temp), "%d\n", value);

    if (len < size) {
        atomic_dec(&counter);
        return -EINVAL;
    }

    if (copy_to_user(buf, temp, size)) {
        atomic_dec(&counter);
        return -EFAULT;
    }

    atomic_inc(&read_count);
    *ppos += size;

    return size;
}

/* Write an integer to the counter */
static ssize_t counter_write(struct file *file,
                             const char __user *buf,
                             size_t len,
                             loff_t *ppos)
{
    char temp[32];
    int value;
    int ret;

    if (len == 0 || len >= sizeof(temp))
        return -EINVAL;

    if (copy_from_user(temp, buf, len))
        return -EFAULT;

    temp[len] = '\0';

    ret = kstrtoint(temp, 10, &value);
    if (ret)
        return -EINVAL;

    if (value < 0)
        return -EINVAL;

    atomic_set(&counter, value);
    atomic_inc(&write_count);

    return len;
}

/* Display statistics through procfs */
static int counter_proc_show(struct seq_file *m, void *v)
{
    seq_puts(m, "Counter Device Statistics\n");
    seq_puts(m, "=========================\n");

    seq_printf(m, "Current counter : %d\n",
               atomic_read(&counter));
    seq_printf(m, "Total reads     : %d\n",
               atomic_read(&read_count));
    seq_printf(m, "Total writes    : %d\n",
               atomic_read(&write_count));

    return 0;
}

static int counter_proc_open(struct inode *inode,
                             struct file *file)
{
    return single_open(file, counter_proc_show, NULL);
}

static const struct proc_ops counter_proc_fops = {
    .proc_open    = counter_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* Device operations */
static const struct file_operations counter_fops = {
    .owner  = THIS_MODULE,
    .read   = counter_read,
    .write  = counter_write,
    .llseek = no_llseek,
};

/* Misc device registration */
static struct miscdevice counter_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &counter_fops,
};

/* Initialize driver */
static int __init counter_init(void)
{
    int ret;

    atomic_set(&counter, 0);
    atomic_set(&read_count, 0);
    atomic_set(&write_count, 0);

    ret = misc_register(&counter_miscdev);
    if (ret) {
        pr_err("counter: misc_register failed: %d\n", ret);
        return ret;
    }

    if (!proc_create(PROC_NAME, 0444, NULL,
                     &counter_proc_fops)) {
        pr_err("counter: proc_create failed\n");
        misc_deregister(&counter_miscdev);
        return -ENOMEM;
    }

    pr_info("counter: driver loaded successfully\n");
    return 0;
}

/* Cleanup driver */
static void __exit counter_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    misc_deregister(&counter_miscdev);

    pr_info("counter: driver unloaded\n");
}

module_init(counter_init);
module_exit(counter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Cuong Thinh - YOUR_STUDENT_ID");
MODULE_DESCRIPTION("Atomic counter misc device with procfs");
MODULE_VERSION("1.0");
