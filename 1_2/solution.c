#include <linux/module.h>
#include <checker.h>

int init_module(void) {
	call_me("Hello from my module!");
}

void cleanup_module(void) {}

MODULE_LICENSE("GPL");
