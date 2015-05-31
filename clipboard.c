#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/list.h>

/*
 * Simple kernel-based, LIFO clipboard module.
 *
 * This module implements a character device which
 * functions as a clipboard for copy/paste operations.
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Coffill <dcoffill@cs.ucsb.edu>");
MODULE_DESCRIPTION("Kernel-based, file system device for in-memory copy/paste");

static unsigned long buffer_size = 8192;
static list_head cp_string;

module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));

struct cp_string {
	struct list_head list;
	char string[1024]; // String in clipboard
};

static struct file_operations clipboard_fops = {
	.owner = THIS_MODULE,
	.open = clipboard_open,
	.read = clipboard_read,
	.write = clipboard_write,
	.release = clipboard_close,
	.llseek = noop_llseek
};

static int clipboard_open(struct inode *inode, struct file *file)
{

}

static struct miscdevice clipboard_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "clipboard",
	.fops = &clipboard_fops
};

static int __init clipboard_init(void)
{
	if (!buffer_size)
		return -1;

	misc_register(&clipboard_misc_device);
	printk(KERN_INFO "clipboard device has been registered with buffer size %lu bytes\n",
		   	buffer_size);
	return 0;
}

static void __exit clipboard_exit(void)
{
	misc_deregister(&clipboard_misc_device);
	printk(KERN_INFO "clipboard device has been unregistered\n");
}

module_init(clipboard_init);
module_exit(clipboard_exit);
