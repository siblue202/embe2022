#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include "hw_driver.h"

#define     KERNEL_TIMER_NAME       "/dev/dev_driver"

int main(int argc, char **argv){
    int timer_fd;
    struct Ioctl_info set_option;
    struct Ioctl_info command;

    if (argc != 4) {
        printf("Usage : [TIMER_INTERVAL] [TIMER_CNT] [TIMER_INIT]\n");
        return -1;
    }

    timer_fd = open(KERNEL_TIMER_NAME, O_WRONLY);
    if (timer_fd < 0) {
        printf("Open Failured! \n");
        return -1;
    }

    set_option.interval = atoi(argv[1]);
    set_option.cnt = atoi(argv[2]);
    set_option.init = atoi(argv[3]);

    printf("[TIMER_INTERVAL] : %d \n", set_option.interval);
    printf("[TIMER_CNT] : %d \n", set_option.cnt);
    printf("[TIMER_INIT] : %d \n", set_option.init);

    if (ioctl(timer_fd, SET_OPTION, &set_option) < 0) {
        printf("Error : SET_OPTION\n");
    }

    if (ioctl(timer_fd, COMMAND, &command) < 0) {
        printf("Error : COMMAND\n");
    }

    close(timer_fd);

    return 0;
}