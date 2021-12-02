#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>

static dev_t first;
static unsigned int count = 1;
static int my_major = 240, my_minor = 0;
static struct cdev *my_cdev;
static char* kbuf;

#define MYDEV_NAME "solution_node"
#define KBUF_SIZE (size_t)(1024 * sizeof(char))

static int counter_open = 0;
static int counter_size_data = 0;

static int mychrdev_open(struct inode *inode, struct file *file) {
    counter_open++;
    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file) {
    return 0;
}

static int mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    int nbytes = snprintf(kbuf, lbuf, "%ld %ld\n", counter_open, counter_size_data);
    int written = nbytes - *ppos - copy_to_user(buf, kbuf + *ppos, nbytes - *ppos);
    *ppos += written;
    return written;
}

static int mychrdev_write(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    counter_size_data += lbuf;
    *ppos = lbuf;
    return lbuf;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = mychrdev_read,
    .write = mychrdev_write,
    .open = mychrdev_open,
    .release = mychrdev_release
};

static int __init init_function(void)
{
    kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
    first = MKDEV(my_major, my_minor);
    register_chrdev_region(first, count, MYDEV_NAME);
    my_cdev = cdev_alloc();
    cdev_init(my_cdev, &mycdev_fops);
    cdev_add(my_cdev, first, count);
    return 0;
}

static void __exit exit_function(void)
{
    if (my_cdev) {
        cdev_del(my_cdev);
    }
    kfree(kbuf);
    unregister_chrdev_region(first, count);
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
