#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>


#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

static int my_major = 240;
static char* kbuf;

#define MYDEV_NAME "solution_node"
#define KBUF_SIZE (size_t)(1024 * sizeof(char))

static int sum_length = 0;
static int sum_numbers = 0;

static int mychrdev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file) {
    return 0;
}

static int mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    int nbytes = snprintf(kbuf, lbuf, "%d %d\n", sum_length, sum_numbers);
    int written = nbytes - *ppos - copy_to_user(buf, kbuf + *ppos, nbytes - *ppos);
    *ppos += written;
    return written;
}

static int mychrdev_write(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    return 0;
}

static int mychrdev_ioctl(struct file* file, unsigned int cmd, unsigned long arg) {
    char str[256];
    strncpy_from_user(str, (char*)arg, 256);
    if (cmd ==  SUM_LENGTH) {
        sum_length += strlen(str);
        return sum_length;
    }
    if (cmd == SUM_CONTENT) {
        int sum;
        kstrtol(str, 10, &sum);
        sum_numbers += sum;
        return sum_numbers;
    }
    return 0;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = mychrdev_read,
    .write = mychrdev_write,
    .open = mychrdev_open,
    .release = mychrdev_release,
    .unlocked_ioctl = mychrdev_ioctl
};

static int __init init_function(void)
{
    kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
    register_chrdev(my_major, MYDEV_NAME, &mycdev_fops);

    return 0;
}

static void __exit exit_function(void)
{
    kfree(kbuf);
    unregister_chrdev(my_major, MYDEV_NAME);
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
