obj-m = stopwatch_driver.o 
CC := arm-none-linux-gnueabi-gcc

KDIR    :=/work/achroimx_kernel
PWD := $(shell pwd)

all: driver
#all: driver 

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf stopwatch_application