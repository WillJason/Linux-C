#KERNELRELEASE是在内核源码的顶层Makefile中定义的一个变量，在第一次读取执行此Makefile时，
#KERNELRELEASE没有被定义，所以make将读取执行else之后的内容，如果make的目标是clean，直接执行clean操作，然后结束。
ifneq ($(KERNELRELEASE),)

obj-m := mini2440_buttons.o

else
	
KDIR := /home/project/mp3/SDK-MP3/kernel/linux-2.6.29
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers app-key

endif
