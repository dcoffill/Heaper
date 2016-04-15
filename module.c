#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/list.h>
#include <linux/hashtable.h>
#include <linux/sched.h>
#include "heap.h"
#include <linux/mutex.h>


/*
 * Simple kernel-based min-heap
 *
 * This module implements a character device which
 * returns the shortest string previously written to it
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Coffill <dcoffill@cs.ucsb.edu>");
MODULE_DESCRIPTION("Kernel-based, file system device to return minimum string written to it");

static unsigned long buffer_size = 8192;

module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));

static struct heap *heap;

static int heaper_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int heaper_read(struct file *file, char __user *out, size_t size, loff_t *off)
{
	return 0;
}

static int heaper_write(struct file *file, const char __user *in, size_t size, loff_t *off)
{

	return 0;
}

static int heaper_close(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations heaper_fops = {
	.owner = THIS_MODULE,
	.open = heaper_open,
	.read = heaper_read,
	.write = heaper_write,
	.release = heaper_close,
	.llseek = noop_llseek
};

static struct miscdevice heaper_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "heaper",
	.fops = &heaper_fops
};

static int __init heaper_init(void)
{
	if (!buffer_size)
		return -1;

	misc_register(&heaper_misc_device);
	heap = init_heap(1024);
	printk(KERN_INFO "heaper device has been registered with buffer size %lu bytes\n",
		   	buffer_size);
	printk(KERN_INFO "UID: %d\n", get_current_user()->uid.val);
	return 0;
}

static void __exit heaper_exit(void)
{
	misc_deregister(&heaper_misc_device);
	printk(KERN_INFO "heaper device has been unregistered\n");
}

module_init(heaper_init);
module_exit(heaper_exit);
