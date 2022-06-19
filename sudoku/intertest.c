#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

unsigned char quit = 0;

void user_signal1(int sig){
    quit = 1;
}

int main(void){
	int fd;
	int buf;
    int result;

	fd = open("/dev/inter", O_RDWR);
	if(fd < 0) {
		perror("/dev/inter error");
		exit(-1);
	} else {
        printf("< inter Device has been detected > \n");
    }

    size_t buf_size = sizeof(buf);

    (void)signal(SIGINT, user_signal1);
	while(!quit){
        usleep(400000);

        read(fd, &buf, buf_size);
        //printf("read result : %c\n", buf);
        if (buf == 0){
            result = 1;
        } else if (buf == 1){
            result = 2;
        } else if (buf == 2){
            result = 3;
        } else if (buf == 3){
            result = 4;
        } else {
            result = 0;
        }

        printf("read result : %d\n", result);

    }

	close(fd);

	return 0;
}
