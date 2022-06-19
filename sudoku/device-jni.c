#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include "android/log.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define LOG_TAG "jniTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

jint JNICALL Java_com_example_androidex_SudokuActivity_readKey(JNIEnv *env, jobject this){
    char buff;
    int result;
    char cmp_1 = '1';
    char cmp_2 = '2';
    char cmp_3 = '3';
    char cmp_4 = '4';


    int fd = open("/dev/inter", O_RDWR);
    if (fd<0){
        return -1;
    }

    read(fd, &buff, 1);

    if (strcmp(&buff,&cmp_1) == 0){
        result = 1;
    } else if (strcmp(&buff,&cmp_2) == 0){
        result = 2;
    } else if (strcmp(&buff,&cmp_3) == 0){
        result = 3;
    } else if (strcmp(&buff,&cmp_4) == 0){
        result = 4;
    } else {
        result = 0;
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