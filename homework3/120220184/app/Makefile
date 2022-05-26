CC := arm-none-linux-gnueabi-gcc

KDIR    :=/work/achroimx_kernel
PWD := $(shell pwd)

all: app

app:
	arm-none-linux-gnueabi-gcc -static -o stopwatch_application stopwatch_application.c
	
clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
	rm -rf stopwatch_application