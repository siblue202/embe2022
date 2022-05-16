#include "output.h"

void output_process(int shm_out){
    int terminated = FALSE;

    struct shm_info_out *shm_addr = (struct shm_info_out *)shmat(shm_out, NULL, 0);
    
    while(!terminated) {
        printf("output_process : 1\n"); // debug

        if(shm_addr->killed == TRUE) {
            shm_addr->fnd_data = 0;
            shm_addr->led_data = 0;
            terminated = TRUE;
        }


        printf("output_process : 2 -> %d\n", shm_addr->init); // debug
        // if main process change mode, another mode will be init mode
        if (shm_addr->init == TRUE) {
            printf("output_process : 2\n"); // debug

            shm_addr->init = FALSE;

            switch (shm_addr->oper_mode) {
            case CLOCK:
                shm_addr->led_data = LED_1;
                break;
            case COUNTER:
                shm_addr->fnd_data = 0;
                shm_addr->led_data = LED_1;
                break;
            case TEXT_EDITOR:
                /* code */
                break;
            case DRAW_BOARD:
                /* code */
                break;
            
            default:
                break;
            }

            // display init mode
            fnd_device(shm_addr->fnd_data);
            led_device(shm_addr->led_data);
        }

        // display output result
        printf("output_process : 3 -> %d\n", shm_addr->oper_mode); // debug
        printf("output_process : 3\n"); // debug
        switch (shm_addr->oper_mode) {
            case CLOCK:
                fnd_device(shm_addr->fnd_data);
                led_device(shm_addr->led_data);
                break;
            case COUNTER:
                fnd_device(shm_addr->fnd_data);
                led_device(shm_addr->led_data);
                break;
            case TEXT_EDITOR:
                /* code */
                break;
            case DRAW_BOARD:
                /* code */
                break;
        
            default:
             break;
        }

        printf("output_process : 4\n"); // debug
        usleep(10000); // sleep 10ms
    }
    
    shmdt(shm_addr);

    return;
}

void fnd_device(int data){
    int fd_fnd, i;
    unsigned char fnd_data[4];
    unsigned char fnd_retval;

    fd_fnd = open(FND_DEVICE, O_RDWR);
    if (fd_fnd < 0) {
        printf("Device open error : %s\n",FND_DEVICE);
    }

    for (i=3; i >= 0; i--){
        fnd_data[i] = data % 10;
        data /= 10;
    }

    fnd_retval=write(fd_fnd, &fnd_data, 4);	
    if(fnd_retval < 0) {
        printf("Write Error!\n");
        return ;
    }

    close(fd_fnd);
}

void led_device(unsigned char data){
    int fd_led, i;

	unsigned long *fpga_addr = 0;
	unsigned char *led_addr = 0;

	fd_led = open(LED_DEVICE, O_RDWR | O_SYNC);
	if (fd_led < 0) {
		printf("Device open error : %s\n", LED_DEVICE);
	}

	fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd_led, FPGA_BASE_ADDRESS);
	if (fpga_addr == MAP_FAILED)
	{
		printf("mmap error!\n");
		close(fd_led);
	}
	
	led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
    *led_addr = data;
	
	munmap(led_addr, 4096);
	close(fd_led);
	return ;
}