#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "android/log.h"
#include <unistd.h>
#include <fcntl.h>

#define LOG_TAG "jniTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

jint JNICALL Java_com_example_androidex_SudokuActivity_readKey(JNIEnv *env, jobject this){
    int result;
    int buf;
    unsigned char dip_sw_buff = 0;

    int fd = open("/dev/fpga_dip_switch", O_RDWR);
    if (fd<0){
        close(fd);
        return -1;
    }

    read(fd, &dip_sw_buff, 1);
    buf = dip_sw_buff;

    if (buf == 0){
        result = 0;
    } else {
        result = 1;
    }
    
    close(fd);
    return result;
}

jint JNICALL Java_com_example_androidex_SudokuActivity_readSwitch(JNIEnv *env, jobject this){
    unsigned char push[9];
    int result;

    int fd = open("/dev/fpga_push_switch", O_RDWR);
    if (fd<0){
        return -1;
    }

    int buff_size = sizeof(push);
    read(fd, push, buff_size);

    if (push[0]){
        result = 1;
    } else if (push[1]){
        result = 2;
    } else if (push[2]){
        result = 3;
    } else if (push[3]){
        result = 4;
    } else if (push[4]){
        result = 5;
    } else if (push[5]){
        result = 6;
    } else if (push[6]){
        result = 7;
    } else if (push[7]){
        result = 8;
    } else if (push[8]){
        result = 9;
    } else {
        result = 0;
    }

    close(fd);
    return result;
}