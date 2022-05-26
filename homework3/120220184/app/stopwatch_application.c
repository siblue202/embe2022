#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <asm/ioctl.h>

#include "./../stopwatch_ioctl.h"					// IOCTL setting header file

#define     KERNEL_STOPWATCH_NAME       "/dev/stopwatch"

int main(int argc, char **argv){
    int stopwatch_fd;
    int i;

    stopwatch_fd = open(KERNEL_STOPWATCH_NAME, O_WRONLY);
    if (stopwatch_fd < 0) {
        printf("Open Failured! \n");
        return -1;
    }

    if (ioctl(stopwatch_fd, COMMAND, NULL) < 0) {
        printf("Error : COMMAND\n");
    }

    close(stopwatch_fd);

    return 0;
}