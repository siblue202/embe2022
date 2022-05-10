#ifndef __HW_DRIVER_H__
#define __HW_DRIVER_H__
#include <asm/ioctl.h>
  
typedef struct Ioctl_info{
       unsigned long interval;
       unsigned long cnt;
       unsigned char init[4];
}Ioctl_info;
   
#define             IOCTL_MAGIC         'T'
#define             SET_OPTION          _IOWR(IOCTL_MAGIC, 2 ,struct Ioctl_info)
#define             COMMAND             _IOWR(IOCTL_MAGIC, 3 ,struct Ioctl_info)
  
#endif