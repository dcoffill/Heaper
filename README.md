Heaper
======

A toy Linux kernel module which implements a min-heap priority queue as a character device, written as a learning exercise.  Heaper will allow users to write() strings into it, and will return the "minimum" (currently based on length) string on a read() call.

Based loosely on [an article by Linux Voice](http://www.linuxvoice.com/be-a-kernel-hacker/).
