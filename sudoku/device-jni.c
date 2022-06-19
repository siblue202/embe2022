#include <jni.h>
#include "android/log.h"
#include <unistd.h>
#include <fcntl.h>

#define LOG_TAG "jniTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

JNIEXPORT jint JNICALL Java_com_example_androidex_SudokuActivity_readKey(JNIEnv *, jobject){

}

JNIEXPORT jint JNICALL Java_com_example_androidex_SudokuActivity_readSwitch(JNIEnv *, jobject){
    
}