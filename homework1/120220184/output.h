
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <string.h>

// fnd
#define MAX_DIGIT 4
#define FND_DEVICE "/dev/fpga_fnd"

// led
#define FPGA_BASE_ADDRESS 0x08000000 //fpga_base address
#define LED_ADDR 0x16 
#define LED_DEVICE "/dev/mem"


void output_process(int shm_out);
void fnd_device(int data);
void led_device(unsigned char data);

