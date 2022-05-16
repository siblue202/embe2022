#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

#define TRUE 1
#define FALSE 0

// IPC_KEY
#define SHARED_KEY_IN (key_t) 0x10 /* shared mem key(input) */
#define SHARED_KEY_OUT (key_t) 0x15 /* shared mem key(output) */
#define SEM_KEY (key_t) 0x20 /* semaphore key */

// readkey
#define KEY_BACK 158
#define KEY_PROG 116
#define KEY_VOL_UP 115
#define KEY_VOL_DOWN 114
#define KEY_IDLE -1
#define KEY_INIT 123 // for entring first operate mode(clock)

// clock flag
#define TRUNING 3
#define ADD_MIN 4

// led
#define LED_1 128
#define LED_2 64
#define LED_3 32
#define LED_4 16

// operating mode 
enum OPERATING_MODE {
    CLOCK,
    COUNTER,
    TEXT_EDITOR,
    DRAW_BOARD
};

// clock mode
enum CLOCK_MODE {
    CLOCK_EDIT,
    CLOCK_DEFAULT
};

// counter mode
enum COUNTER_MODE {
    COUNTER_DECIMAL,
    COUNTER_OCT,
    COUNTER_QUAT,
    COUNTER_BIN
};


// input shared memory format
struct shm_info_in {
    int key_code; // 158(back), 116(prog), 115(vol+), 114(vol-), -1(idle)
    int killed; // 1(killed), 0(alive)
    unsigned char switch_key[9]; // 1~9
    int pid_execute; // child pid that process operating mode 
};

// output shared memory format
struct shm_info_out {
    int fnd_data; // 0000
    unsigned char led_data; 
    unsigned char dot_dat[10];
    int oper_mode; // enum OPERATING_MODE
    int init; // 1(modechange_init), 0
    int killed; // 1(killed), 0(alive)
};



void main_process(int shm_in, int shm_out);
// void execute_process(int shm_in, int shm_out, enum OPERATING_MODE * oper_mode);
void mode_change(struct shm_info_out * out_data, enum OPERATING_MODE * oper_mode, int key_code);