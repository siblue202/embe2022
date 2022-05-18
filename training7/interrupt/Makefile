obj-m := interrupt.o

KDIR := /work/achroimx_kernel/
PWD := $(shell pwd)

all : driver app

driver :
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules 

app :
	arm-none-linux-gnueabi-gcc -static -o intertest intertest.c

clean : 
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.*
	rm -f *.cmd
	rm -f *.symvers
	rm -f *.order
