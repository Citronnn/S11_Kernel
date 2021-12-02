#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>


static dev_t first;
static unsigned int count = 1;
static int my_minor = 0;
static struct cdev *my_cdev;
static char* kbuf;
static struct class *my_class;

#define MYDEV_NAME "solution_node"
#define KBUF_SIZE (size_t)(1024 * sizeof(char))

static char *node_name;
module_param(node_name, charp, 0);

static int mychrdev_open(struct inode *inode, struct file *file) {
    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file) {
    return 0;
}

static int mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    int nbytes = snprintf(kbuf, lbuf, "%d\n", MAJOR(first));
    int written = nbytes - *ppos - copy_to_user(buf, kbuf + *ppos, nbytes - *ppos);
    *ppos += written;
    return written;
}

static int mychrdev_write(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
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
    alloc_chrdev_region(&first, my_minor, 1, MYDEV_NAME);
    my_cdev = cdev_alloc();
    cdev_init(my_cdev, &mycdev_fops);
    cdev_add(my_cdev, first, count);

    my_class = class_create(THIS_MODULE, "my_class");
    device_create(my_class, NULL, first, "%s", node_name);
    return 0;
}

static void __exit exit_function(void)
{
    device_destroy(my_class, first);
    class_destroy(my_class);
    if (my_cdev) {
        cdev_del(my_cdev);
    }
    kfree(kbuf);
    unregister_chrdev_region(first, count);
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
