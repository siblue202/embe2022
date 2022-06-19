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
	char buf;
    int result;

    char cmp_1 = '1';
    char cmp_2 = '2';
    char cmp_3 = '3';
    char cmp_4 = '4';

	fd = open("/dev/inter", O_RDWR);
	if(fd < 0) {
		perror("/dev/inter error");
		exit(-1);
	} else {
        printf("< inter Device has been detected > \n");
    }

    (void)signal(SIGINT, user_signal1);
	while(!quit){
        usleep(400000);

        read(fd, &buf, 1);
        //printf("read result : %c\n", buf);
        if (strcmp(&buf,&cmp_1) == 0){
            result = 1;
        } else if (strcmp(&buf,&cmp_2) == 0){
            result = 2;
        } else if (strcmp(&buf,&cmp_3) == 0){
            result = 3;
        } else if (strcmp(&buf,&cmp_4) == 0){
            result = 4;
        } else {
            result = 0;
        }

        printf("read result : %d\n", result);

    }

	close(fd);

	return 0;
}
