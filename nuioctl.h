#ifndef __NUIOCTL_H__
#define __NUIOCTL_H__

#include <linux/ioctl.h>

/*
 * http://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN567
 * http://www.mjmwired.net/kernel/Documentation/ioctl-number.txt
 */

#define NUIOCTL_ID 'N'

struct nuioctl {
  void *address;
  int length;
};

// EXM : Execution manager

#define IOCTL_EXM_BASE 0xA0
#define IOCTL_EXM_NEW (IOCTL_EXM_BASE + 0)

#define NUIOCTL_EXM_NEW _IOW(NUIOCTL_ID, IOCTL_EXM_NEW, struct nuioctl *)

#define DEVICE_FILE_NAME "nugpgpu"

#endif
