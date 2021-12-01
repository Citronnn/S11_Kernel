#include <linux/module.h>
#include <linux/slab.h>
#include <checker.h>


void *void_ptr;
int *int_ptr;
struct device *device_ptr;

static int __init init_function(void)
{
	ssize_t void_size = get_void_size();
    ssize_t array_size = get_int_array_size();
    ssize_t device_size = sizeof(struct device);
    void_ptr = kmalloc_array( void_size, sizeof(char), GFP_KERNEL );
    int_ptr = kmalloc_array( array_size, sizeof(int), GFP_KERNEL );
    device_ptr = kmalloc( device_size, GFP_KERNEL );
    submit_void_ptr(void_ptr);
    submit_int_array_ptr(int_ptr);    
    submit_struct_ptr(device_ptr);
	return 0;
}

static void __exit exit_function(void)
{
    checker_kfree( void_ptr );
    checker_kfree( int_ptr );
    checker_kfree( device_ptr );
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
