Heaper
======

A toy Linux kernel module which implements a min-heap priority queue as a character device, written as a learning exercise.  Heaper will allow users to instantiate an instance with open(), write() strings into it, and will return the "minimum" (currently based on length) string on a read() call.  Finally, on close(), the insance of Heaper will deallocate itself.

In a way, it behaves somewhat like a priority-queue meets a UNIX pipe.  The example.c file contains an example program where a parent and child process use Heaper to pass prioritized data (grabbing the shortest string currently in the buffer).

`module.c` is based on code from [an article by Linux Voice](http://www.linuxvoice.com/be-a-kernel-hacker/).

Licensed under the GPLv2.

Future work
===========
Try to break the UNIX file abstraction less.  Currently, all given read() calls are treated the same in that they consume an element from Heaper's queue.  Similarly, a write() inserts something into Heaper.  This is all fine and good if code is written to take advantage of Heaper's quirks (such as example.c), but for example, an implementation of `cat` that reads a single character at a time would consume an entire entry in the priority queue with each single-character call to read, rather than grabbing the min-entry one character at a time.

Ideally, at some point this module will implement some kind of buffered read, such that small reads can get portions of the min entry without having to read the whole thing at once or risk removing it and losing it forever.  Similarly, a write should also implement some kind of buffering, such that a program can write a single queue entry with multiple calls to write().
