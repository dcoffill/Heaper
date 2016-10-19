# Makefile obtained from http://www.linuxvoice.com/be-a-kernel-hacker/
obj-m := heaper.o
heaper-objs := heap.o module.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
