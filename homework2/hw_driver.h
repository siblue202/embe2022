#ifndef __HW_DRIVER_H__
#define __HW_DRIVER_H__
#include <asm/ioctl.h>
  
struct ioctl_info {
       unsigned long interval;
       unsigned long cnt;
       unsigned char[4] init;
};
   
#define             IOCTL_MAGIC         'T'
#define             SET_OPTION          _IOWR(IOCTL_MAGIC, 2 ,struct ioctl_info)
#define             COMMAND             _IOWR(IOCTL_MAGIC, 3 ,struct ioctl_info)
  
#endif