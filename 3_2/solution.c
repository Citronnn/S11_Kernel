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

struct session_kbuf {
    int id;
    char *kbuf;
    int size;
};


#define MYDEV_NAME "solution_node"
#define KBUF_SIZE (size_t)(256 * sizeof(char))

static int counter_open = 0;

static int mychrdev_open(struct inode *inode, struct file *file) {
    struct session_kbuf* kbuf = kmalloc(sizeof(struct session_kbuf), GFP_KERNEL);
    kbuf->id = counter_open;
    kbuf->kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL); 
    kbuf->size = sprintf(kbuf->kbuf, "%d", kbuf->id);
    counter_open++;
    file->private_data = kbuf;
    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file) {
    struct session_kbuf* kbuf = file->private_data;
    if (kbuf) {
        kfree(kbuf->kbuf);
    }
    kfree(kbuf);
    file->private_data = NULL;    
    return 0;
}

static int mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    struct session_kbuf* kbuf = file->private_data;
    int nbytes = kbuf->size - *ppos - copy_to_user(buf, kbuf->kbuf + *ppos, kbuf->size - *ppos);
    *ppos += nbytes;
    return nbytes;
}

static int mychrdev_write(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    struct session_kbuf* kbuf = file->private_data;
    int nbytes = lbuf - copy_from_user(kbuf->kbuf + *ppos, buf, lbuf);
    *ppos += nbytes;
    if (*ppos > kbuf->size) {
        kbuf->size = *ppos;
    }
    return lbuf;
}

static loff_t mychrdev_lseek(struct file *file, loff_t offset, int orig) {
    loff_t testpos;
    struct session_kbuf* kbuf = file->private_data;
    switch(orig) {
        case SEEK_SET:
            testpos = offset;
            break;
        case SEEK_CUR:
            testpos = file->f_pos + offset;
            break;
        case SEEK_END:
            testpos = kbuf->size + offset;
            break;
        default:
            return -EINVAL;
    }
    testpos = testpos < KBUF_SIZE ? testpos : KBUF_SIZE;
    testpos = testpos >= 0 ? testpos : 0;
    file->f_pos = testpos;
    return testpos;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .read = mychrdev_read,
    .write = mychrdev_write,
    .open = mychrdev_open,
    .release = mychrdev_release,
    .llseek = mychrdev_lseek
};

static int __init init_function(void)
{
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
    unregister_chrdev_region(first, count);
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
