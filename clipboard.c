#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Coffill <dcoffill@cs.ucsb.edu>");
MODULE_DESCRIPTION("Kernel-based, file system device for in-memory copy/paste");
static unsigned long buffer_size = 8192;
module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));

static int __init clipboard_init(void)
{
	if (!buffer_size)
		return -1;

	printk(KERN_INFO "clipboard device has been registered with buffer size %lu bytes\n",
		   	buffer_size);
	return 0;
}

static void __exit clipboard_exit(void)
{
	printk(KERN_INFO "clipboard device has been unregistered\n")
}

module_init(clipboard_init);
module_exit(clipboard_exit);
