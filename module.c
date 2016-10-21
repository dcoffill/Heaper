#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include "heap.h"
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


/*
 * Simple kernel-based min-heap
 *
 * This module implements a character device which
 * returns the shortest string previously written to it
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Coffill <dcoffill@cs.ucsb.edu>");
MODULE_DESCRIPTION("Kernel-based, file system device to return shortest string written to it");

static unsigned long buffer_size = 8192;

module_param(buffer_size, ulong, (S_IRUSR | S_IRGRP | S_IROTH));


static int heaper_open(struct inode *inode, struct file *file)
{
	int result = 0;
	struct heap *heap_ptr;
	heap_ptr = init_heap(buffer_size);
	if (heap_ptr == NULL) {
		result = -ENOMEM;
		goto out;
	}
	file->private_data = heap_ptr;
	printk(KERN_INFO "Allocated heaper instance\n");

out:
	return result;
}

static ssize_t heaper_read(struct file *file, char __user *out, size_t size, loff_t *off)
{
	struct heap *heap = (struct heap *)file->private_data;
	char *string = NULL;
	int result = 0;
	printk(KERN_INFO "Heaper read()\n");

	if (unlikely(heap == NULL)) {
		result = -EFAULT;
		goto out;
	}

	if (mutex_lock_interruptible(&heap->lock)) {
		result = -ERESTART;
		goto out;
	}

	/* Block until heap is no longer empty */
	while (heap->end == 0) {
		mutex_unlock(&heap->lock);
		if (file->f_flags & O_NONBLOCK) {
			result = -EAGAIN;
			goto out;
		}

		if(wait_event_interruptible(heap->read_queue, heap->end != 0)) {
			result = -ERESTART;
			goto out;
		}

		/* Attempt to acquire the lock again */
		if (mutex_lock_interruptible(&heap->lock)) {
			result = -ERESTART;
			goto out;
		}
	}

	string = delete_min(heap);
	printk(KERN_INFO "Retrieved %lu byte string", strlen(string) + 1);
	/* Length of string or requested length from user, whichever is smaller */
	size = min(size, strlen(string) + 1);

	if (copy_to_user(out, string, size)) {
		result = -EFAULT;
		/* something failed, so reinsert string into heap */
		heap_insert(heap, string);
		goto out_unlock;
	}
	result = size;
	kfree(string);

out_unlock:
	mutex_unlock(&heap->lock);
out:
	return result;
}

static ssize_t heaper_write(struct file *file, const char __user *in, size_t size, loff_t *off)
{
	struct heap *heap = (struct heap *)file->private_data;
	char *string = NULL;
	int result = 0;
	if (unlikely(heap->end == heap->size - 1)) {
		result = -ENOBUFS;
		goto out;
	}

	if (unlikely(heap == NULL)) {
		result = -EFAULT;
		goto out;
	}

	if (mutex_lock_interruptible(&heap->lock)) {
		result = -ERESTART;
		goto out;
	}

	string = (char *)kmalloc(size, GFP_KERNEL);

	if (unlikely(string == NULL)) {
		result = -EFAULT;
		goto out_unlock;
	}

	if (copy_from_user(string, in, size)) {
		result = -EFAULT;
		goto out_unlock;
	}
	printk(KERN_INFO "Inserted %s with size %lu\n into Heaper", string, size);

	string[size - 1] = '\0';	/* ensure string is null terminated */
	heap_insert(heap, string);
	wake_up_interruptible(&heap->read_queue);
	result = size;

out_unlock:
	mutex_unlock(&heap->lock);

out:
	return result;
}

static int heaper_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Heaper close()\n");
	free_heap((struct heap *)file->private_data);
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
	if (!buffer_size) {
		return -1;
	}

	misc_register(&heaper_misc_device);
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
