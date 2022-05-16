#include "main.h"

static enum OPERATING_MODE oper_mode;
static enum CLOCK_MODE clock_mode;
static enum COUNTER_MODE counter_mode;

// clock
static int save_time;

// counter
int number, base;

// execute process
static int terminated_execute = FALSE;


/* ================================================================================*/

int main(void){
    int shm_in, shm_out;
    pid_t pid_in, pid_out;

    shm_in = shmget(SHARED_KEY_IN, sizeof(struct shm_info_in), IPC_CREAT|0644);
    shm_out = shmget(SHARED_KEY_OUT, sizeof(struct shm_info_out), IPC_CREAT|0644);

    pid_in = fork();
    printf("main process : fork() pid_in\n");
    if(pid_in == -1) {
        printf("error : fork pid_in\n"); 
        exit(0);

    } else if (pid_in == 0) {
        // start input_process
        printf("main process : fork() 11111111\n");
	input_process(shm_in);
        printf("main process : fork() pid_in completed\n");

    } else {
        // main_process after fork input_process
        pid_out = fork();
        if(pid_out == -1) {
            printf("error : fork pid_in\n");
            exit(0);
        } else if (pid_out == 0) {
            // start output_process
            output_process(shm_out);
        }
    }

    // main_process
    if (pid_in && pid_out) {
        main_process(shm_in, shm_out);
    }

    // wait for children(input, output) to end
    wait(pid_in);
    wait(pid_out);

    // destory shared memory
    shmctl(shm_in, IPC_RMID, NULL);
    shmctl(shm_out, IPC_RMID, NULL);

    return 0;
}

/* ================================================================================*/

void main_process (int shm_in, int shm_out) {
    int i;
    int prev_keycode;
    int cur_keycode = 0;
    int killed = FALSE;
    int sec_count =0;
    int min_count =0;

    oper_mode = CLOCK;
    clock_mode = CLOCK_DEFAULT;
    counter_mode = COUNTER_DECIMAL;


    struct shm_info_in *shm_in_addr = (struct shm_info_in *)shmat(shm_in, NULL, 0);
    struct shm_info_out *shm_out_addr = (struct shm_info_out *)shmat(shm_in, NULL, 0);

    // init input data
    shm_in_addr->key_code = KEY_IDLE;
    for (i=0; i<9; i++){
        shm_in_addr->switch_key[i] = 0;
    }
    shm_in_addr->killed = FALSE;

    // init output data
    shm_out_addr->killed = FALSE;


    while (!killed) {
        printf("1\n"); // debug

        // check diff keycode value of input_process
        prev_keycode = cur_keycode;
        cur_keycode = shm_in_addr->key_code;

        if (prev_keycode != cur_keycode) {
            printf("2\n"); // debug
            switch (cur_keycode) {
                case KEY_BACK:
                    killed = TRUE;
                    shm_in_addr->killed = TRUE;
                    shm_out_addr->killed = TRUE;

                case KEY_PROG:
                    break;

                case KEY_VOL_UP:
                    mode_change(shm_out_addr, &oper_mode,  cur_keycode);
                    break;

                case KEY_VOL_DOWN:
                    mode_change(shm_out_addr, &oper_mode,  cur_keycode);
                    break;

                case KEY_IDLE:
                    break;
            
                case KEY_INIT:
                    // execute_process(shm_in, shm_out, &oper_mode);
                    break;
            
                default:
                    break;
            }
        }

        // count sec_count
        usleep(1000);
        printf("3\n"); // debug
        sec_count += 1;
        if (sec_count == 1000) {
            printf("4\n"); // debug
            min_count += 1;
            sec_count = 0;

            if (oper_mode == CLOCK && clock_mode == CLOCK_EDIT){
                // turn on&off led 3, 4
                clock_process(shm_out_addr, shm_in_addr, &clock_mode, TRUNING);
            }
        }

        if (min_count == 60) {
            printf("4\n"); // debug
            min_count = 0;
            if (oper_mode == CLOCK){
                // add minute
                clock_process(shm_out_addr, shm_in_addr, &clock_mode, ADD_MIN);
            }
        }

        // execute process that operate corresponding opertaion mode
        // execute_process(shm_in, shm_out, &oper_mode);
        if(!terminated_execute){
            terminated_execute = TRUE;
            printf("5\n"); // debug
            int pid_execute;

            pid_execute = fork();
            if(pid_execute == -1) {
                printf("error : fork pid_execute\n"); 
            } else if (pid_execute == 0) {
                printf("6\n"); // debug
                // start execute_process
                struct shm_info_in *shm_in_addr_ex = (struct shm_info_in *)shmat(shm_in, NULL, 0);
                struct shm_info_out *shm_out_addr_ex = (struct shm_info_out *)shmat(shm_out, NULL, 0);
        
                // entering corresponding operating mode
                switch (oper_mode) {
                    case CLOCK:
                        printf("7\n"); // debug
                        clock_process(shm_out_addr_ex, shm_in_addr_ex, &clock_mode, NULL);
                        break;
        
                    case COUNTER:
                        printf("8\n"); // debug
                        counter_process(shm_out_addr_ex, shm_in_addr_ex, &counter_mode, NULL);
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

                shmdt(shm_in_addr_ex);
                shmdt(shm_out_addr_ex);

            } else {
                printf("9\n"); // debug
                wait(pid_execute);
                terminated_execute = FALSE;
                printf("10\n"); // debug
            }

        }
        printf("11\n"); // debug
    }

    shmdt(shm_in_addr);
    shmdt(shm_out_addr);

}

void mode_change(struct shm_info_out * out_data, enum OPERATING_MODE * oper_mode, int key_code) {
    printf("mode change start\n");
    if (*oper_mode == CLOCK){
        save_time = out_data->fnd_data;
    }

    if (key_code == KEY_VOL_UP) {
        // clock -> counter -> texteditor -> drawboard -> clock
        *oper_mode = (*oper_mode + 1) % 4;
    } else if (key_code == KEY_VOL_DOWN) {
        // clock -> drawboard -> texteditor -> counter -> clock
        *oper_mode = (*oper_mode + 3) % 4;
    } 

    out_data->oper_mode = *oper_mode;
    out_data->init = TRUE;

    switch (*oper_mode)
    {
    case CLOCK:
        clock_mode = CLOCK_DEFAULT;
        out_data->fnd_data = save_time;
        break;
    case COUNTER:
        counter_mode = COUNTER_DECIMAL;
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
    printf("mode change done\n");


    return;
}
