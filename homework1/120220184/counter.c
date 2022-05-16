#include "counter.h"

extern int number, base;

void counter_process(struct shm_info_out * out_data, struct shm_info_in * in_data, enum COUNTER_MODE* counter_mode, int flag){
    /*
        mode
    // counter mode
    enum COUNTER_MODE {
    COUNTER_DECIMAL,
    COUNTER_OCT,
    COUNTER_QUAT,
    COUNTER_BIN
    };
    */
    
    int third_num;
    int second_num;
    int first_num;
    
    if (in_data->switch_key[0] == 1) {
        // sw(1) true
        in_data->switch_key[0] = 0;
        *counter_mode = (*counter_mode + 1) % 4;
        switch (*counter_mode) {
        case COUNTER_DECIMAL:
            out_data->led_data = LED_2;
            base = 10;
            break;
        case COUNTER_OCT:
            out_data->led_data = LED_3;
            base = 8;
            break;
        case COUNTER_QUAT:
            out_data->led_data = LED_4;
            base = 4;
            break;
        case COUNTER_BIN:
            out_data->led_data = LED_1;
            base = 2;
            break;
        
        default:
            break;
        }
    } else if (in_data->switch_key[1] == 1) {
        // sw(2) true
        number += base*base;
    } else if (in_data->switch_key[2] == 1) {
        // sw(3) true
        number += base;
    } else if (in_data->switch_key[3] == 1) {
        // sw(4) true
        number += 1;
    }

    // display number
    third_num = number % base;
    second_num = (number / base) % base;
    first_num = ((number / base) / base) % base;

    out_data->fnd_data = first_num * 100 + second_num * 10 + third_num;

    return ;
}
