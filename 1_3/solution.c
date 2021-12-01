#include <linux/module.h>
#include <linux/random.h>
#include <checker.h>


int get_sum(short arr[]) {
	int i = 0;
	int sum = 0;
	for (i = 0; i < 100; i++) {
		sum += arr[i];
	}
	return sum;
}

static int __init init_function(void)
{
	int i = 0;
	int j = 0;
	short arr[100];
	char output[500];
	int local_sum;
	int check_sum;
	CHECKER_MACRO;
	for (j = 0; j < 10; j++) {
		int size = get_random_int() % 90 + 10;
		for (i = 0; i < size; i++) {
			arr[i] = get_random_int() % 100;
		}
		for (i = 0; i < 100 - size; i++) {
			arr[i + size] = 0;
		}
		local_sum = get_sum(arr);
		check_sum = array_sum(arr, 100);
    	generate_output(check_sum, arr, 100, output);
    	if (check_sum == local_sum) {
            printk( KERN_INFO "%s", output);
        } else {
            printk( KERN_ERR "%s", output);
        }
	}

	return 0;
}

static void __exit exit_function(void)
{
	CHECKER_MACRO;
}

 

module_init(init_function);
module_exit(exit_function);

MODULE_LICENSE("GPL");
