#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/rtc.h>

static int my_sys = 0;
static int RTC_IRQ = 8;

static struct kobject *my_kobject;

static int counter = 0;

static irqreturn_t rtc_interrupt(int irq, void* dev_id ) {
    counter++;
    return IRQ_HANDLED;
}

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    return sprintf(buf, "%d\n", counter);
}

static ssize_t my_sys_store(struct kobject *kobj, struct kobj_attribute *attr,
             const char *buf, size_t count)
{
    return count;
}

static struct kobj_attribute my_attribute =
    __ATTR(my_sys, 0664, my_sys_show, my_sys_store);


static struct attribute *attrs[] = {
    &my_attribute.attr,
    NULL,   /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static int __init init_function(void)
{
    int retval;
    my_kobject = kobject_create_and_add("my_kobject", kernel_kobj);
    if (!my_kobject)
        return -ENOMEM;

    /* Create the files associated with this kobject */
    retval = sysfs_create_group(my_kobject, &attr_group);
    if (retval)
        kobject_put(my_kobject);

    if (request_irq(RTC_IRQ, rtc_interrupt, IRQF_SHARED , "rtc", "citron")) {
        printk(KERN_ERR "rtc: cannot register IRQ %dn" , RTC_IRQ);
        return -EIO;
    }
    return retval;
}

static void __exit exit_function(void)
{
    kobject_put(my_kobject);
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
