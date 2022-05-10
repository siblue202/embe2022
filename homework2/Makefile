obj-m = hw_driver.o 
CC := arm-none-linux-gnueabi-gcc

KDIR    :=/work/achroimx_kernel
PWD := $(shell pwd)

all: driver app
#all: driver 

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

app:
	arm-none-linux-gnueabi-gcc -static -o hw_driver_test hw_driver_test.c
	
	
clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf hw_driver_test