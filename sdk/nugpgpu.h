#ifndef __NUGPGPU_H__
#define __NUGPGPU_H__

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "nuioctl.h"

#define NUGPGPU_DEV "/dev/nugpgpu"

static inline int __nugpgpu_dev_open() {
  return open(NUGPGPU_DEV, O_RDONLY);
}

#define NUGPGPU_OPEN_FD __nugpgpu_dev_open()

#endif
