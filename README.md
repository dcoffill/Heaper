clipboard.c
===========

A Linux kernel module which implements a LIFO clipboard character device. A user can redirect text into the device to push it into the clipboard, and keep doing so an arbitrary amount of times.  The user can also, at any time, pop previous-written clipboard copies from the device by reading from it.

Under heavy development and currently unusable.

Based on [an article by Linux Voice](http://www.linuxvoice.com/be-a-kernel-hacker/).
