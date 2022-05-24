#ifndef __STOPWATCH_IOCTL_H__
#define __STOPWATCH_IOCTL_H__
#include <asm/ioctl.h>
   
#define     IOCTL_MAGIC         'T'
#define     COMMAND             _IO(IOCTL_MAGIC, 3)
  
#endif