#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>

static struct kobject *my_kobject;

static int a;
static int b;
static int c[5];

module_param(a, int, 0);
module_param(b, int, 0);
module_param_array(c, int, NULL, 0);

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    int i = 0;
    int sum = a + b;
    for (i = 0; i < 5; i++) {
        sum += c[i];
    }
    return sprintf(buf, "%d\n", sum);
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

    return retval;
}

static void __exit exit_function(void)
{
    kobject_put(my_kobject);
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
