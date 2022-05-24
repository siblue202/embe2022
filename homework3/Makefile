obj-m = stopwatch_driver.o 
CC := arm-none-linux-gnueabi-gcc

KDIR    :=/work/achroimx_kernel
PWD := $(shell pwd)

all: driver app
#all: driver 

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app : 
	arm-none-linux-gnueabi-gcc -static -o stopwatch_driver stopwatch_driver.c	
	
clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf stopwatch_driver