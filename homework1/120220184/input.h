
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/ioctl.h>
#include <signal.h>


#include <linux/input.h>

// readkey MACRO
#define BUFF_SIZE 64

#define KEY_RELEASE 0
#define KEY_PRESS 1



// switch 
#define MAX_BUTTON 9


void input_process(int shm_in);
void check_readkey(int fd_readkey, struct shm_info_in * shm_addr);
void check_switch(int fd_switch, struct shm_info_in * shm_addr);
