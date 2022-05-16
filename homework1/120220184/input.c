#include "input.h"

void input_process(int shm_in){
    int fd_readkey, fd_switch;
    int terminated = FALSE;

    struct shm_info_in *shm_addr = (struct shm_info_in *)shmat(shm_in, NULL, 0);

    char* key_device = "/dev/input/event0"; // readkey
    char* switch_device  = "/dev/fpga_push_switch"; // switch
    printf("input_process : 1\n"); // debug

    if((fd_readkey = open (key_device, O_RDONLY)) == -1) {
		    printf ("%s is not a vaild device.n", key_device);
	}

    if((fd_switch = open (switch_device, O_RDWR)) == -1) {
		    printf ("%s is not a vaild device.n", switch_device);
	}
    printf("input_process : 2 -> %d\n", shm_addr->killed); // debug
    printf("input_process : 2\n"); // debug

    while(terminated != TRUE){

        if(shm_addr->killed == TRUE){
            terminated = TRUE;
        }
        printf("input_process : loop\n"); // debug
        check_readkey(fd_readkey, shm_addr);
        check_switch(fd_switch, shm_addr);
        
        usleep(10000); // sleep 10ms
    }

    shmdt(shm_addr);
    close(fd_readkey);
    close(fd_switch);

    return;
}

void check_readkey(int fd_readkey, struct shm_info_in * shm_addr){
    struct input_event ev[BUFF_SIZE];
    // type : type of event : 1
    // code : event code ex) 158(back), 116(prog), 115(vol+), 114(vol-)
    // value : value of event : 1(press), 0(release)
    int rd, value, size = sizeof(struct input_event);
    
    rd = read (fd_readkey, ev, size * BUFF_SIZE);

    value = ev[0].value;

    if (rd > 0 && ev[0].value == KEY_PRESS){ // Only read the key press event
		switch (ev[0].code) {
            case KEY_BACK :
                shm_addr->key_code = ev[0].code;
                break;
            case KEY_PROG :
                shm_addr->key_code = ev[0].code;
                break;
            case KEY_VOL_UP :
                shm_addr->key_code = ev[0].code;
                break;
            case KEY_VOL_DOWN :
                shm_addr->key_code = ev[0].code;
                break;
        }
	} else {
        shm_addr->key_code = KEY_IDLE;
    }

    return;
}

void check_switch(int fd_switch, struct shm_info_in * shm_addr){
    int buff_size, i, j;
    unsigned char push_sw_buff[MAX_BUTTON];
    unsigned char prev_sw_buff[MAX_BUTTON];
    unsigned char result[MAX_BUTTON];

    buff_size = sizeof(unsigned char) * 9;

    memset(result, 0, buff_size);

    for(i=0; i<5000 ; i++){
        memcpy(prev_sw_buff, push_sw_buff, buff_size);
        read(fd_switch, push_sw_buff, buff_size);

        for (j = 0; j < 9; j++){
            if(push_sw_buff[j] - prev_sw_buff[j] == KEY_PRESS){
                result[j] = KEY_PRESS;
            }
        }
    }

    memcpy(shm_addr->switch_key, result, buff_size);

    return;
}