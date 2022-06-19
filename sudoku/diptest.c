#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

unsigned char quit = 0;

void user_signal1(int sig){
    quit = 1;
}

int main(void){
	int fd;
    int result;
	unsigned char dip_sw_buff = 0;

	fd = open("/dev/fpga_dip_switch", O_RDWR);
	if(fd < 0) {
		perror("/dev/inter error");
		exit(-1);
	} else {
        printf("< inter Device has been detected > \n");
    }

    (void)signal(SIGINT, user_signal1);
	while(!quit){
        usleep(400000);

        read(fd, &dip_sw_buff, 1);
        result = dip_sw_buff;

        printf("read result : 0x%02X\n", dip_sw_buff);
        printf("result = %d\n", result);

    }

	close(fd);

	return 0;
}
