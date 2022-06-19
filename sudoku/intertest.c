#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
int main(void){
	int fd;
	char buf;

	fd = open("/dev/inter", O_RDWR);
	if(fd < 0) {
		perror("/dev/inter error");
		exit(-1);
	}
        else { printf("< inter Device has been detected > \n"); }
	
    read(fd, &buf, 1);

	close(fd);

	return 0;
}
