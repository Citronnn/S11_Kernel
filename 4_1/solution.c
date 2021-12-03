#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/list.h>

static int my_sys = 0;

static struct kobject *my_kobject;

static ssize_t my_sys_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    struct list_head* head = &(THIS_MODULE->list);
    struct list_head* iter = head;

    char *names[100];
    names[0] = THIS_MODULE->name;

    int counter = 1;

    list_for_each(iter, head) {
        struct module* my_module = list_entry(iter, struct module, list);
        names[counter] = my_module->name;
        counter++;
    }

    int i, j;
    int counter_without_trash = counter - 1;
    for (i = 0; i < counter_without_trash - 1; i++) {
        for (j = 0; j < counter_without_trash - 1 - i; j++) {
            if (strcmp(names[j], names[j + 1]) > 0) {
                char *tmp = names[j];
                names[j] = names[j + 1];
                names[j + 1] = tmp;
            }
        }
    }

    int ret = 0;
    for (i = 0; i < counter_without_trash; i++) {
        ret += sprintf(ret + buf, "%s\n", names[i]);
    }
    return ret;
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
