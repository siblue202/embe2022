#include "clock.h"

void clock_process(struct shm_info_out * out_data, struct shm_info_in * in_data, enum CLOCK_MODE* clock_mode, int flag){
    if (flag == TRUNING && *clock_mode == CLOCK_EDIT) {
        if (out_data->led_data == LED_3) {
            out_data->led_data = LED_4;
        } else {
            out_data->led_data = LED_3;
        }
    }

    if (flag == ADD_MIN) {
        int cur_time = out_data->fnd_data;
        int hour_time = cur_time / 100;
        int min_time = cur_time % 100;
        if (cur_time >= 2359) {
            out_data->fnd_data = 0;
        } else if (min_time >= 59) {
            out_data->fnd_data = cur_time + 41;
        } else {
            out_data->fnd_data = cur_time + 1;
        }
    }

    if (*clock_mode == CLOCK_DEFAULT) {
        if (in_data->switch_key[0] == 1) {
            *clock_mode = CLOCK_EDIT;
            out_data->led_data = LED_1;
            in_data->switch_key[0] = 0;
            printf("(executor process) clock_mode : default, switch[0]: 1");
        }

    } else if(*clock_mode == CLOCK_EDIT) {
        if (in_data->switch_key[0] == 1) {
            *clock_mode = CLOCK_DEFAULT;
            out_data->led_data = LED_1;
            in_data->switch_key[0] = 0;
            printf("(executor process) clock_mode : clock_edit, switch[0] : 1");

        } else if (in_data->switch_key[1] == 1) {
            out_data->fnd_data = get_curtime();
            in_data->switch_key[1] = 0;
            printf("(executor process) clock_mode : clock_edit, switch[1] = 1");
        
        } else if (in_data->switch_key[2] == 1) {
            int cur_time = out_data->fnd_data;
            int hour_time = cur_time / 100;
            int min_time = cur_time % 100;
            
            if(hour_time >= 23) {
                out_data->fnd_data = min_time;
            } else {
                out_data->fnd_data = cur_time + 100;
            }
            printf("(executor process) clock_mode : clock_edit, switch[2] = 1");
            
        } else if (in_data->switch_key[3] == 1) {
            int cur_time = out_data->fnd_data;
            int hour_time = cur_time / 100;
            int min_time = cur_time % 100;
            if (cur_time >= 2359) {
                out_data->fnd_data = 0;
            } else if (min_time >= 59) {
                out_data->fnd_data = cur_time + 41;
            } else {
                out_data->fnd_data = cur_time + 1;
            }
            in_data->switch_key[3] = 0;
            printf("(executor proces) clock_mode : clock_edit, switch[3] = 1");
        }

    }
}

int get_curtime(){
    time_t curTime = time(NULL);
    struct tm pLocal =  * localtime(&curTime);

    return pLocal.tm_hour * 100 +pLocal.tm_min;
}

