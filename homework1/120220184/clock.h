
#include "main.h"
#include <time.h>
#include <string.h>

void clock_process(struct shm_info_out * out_data, struct shm_info_in * in_data, enum CLOCK_MODE* clock_mode, int flag);

int get_curtime();
