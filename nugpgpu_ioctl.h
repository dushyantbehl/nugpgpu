#ifndef __NUGPGPU_IOCTL_H__
#define __NUGPGPU_IOCTL_H__

#include <linux/fs.h>

#include "nugpgpu_drv.h"
#include "nugpgpu_vendor.h"

int nugpgpu_fops_init(struct nugpgpu_private *dev_priv);
void nugpgpu_fops_finish(struct nugpgpu_private *dev_priv);
int nugpgpu_fops_open(struct inode *ngpgpu_inode,
                      struct file *nugpgpu_file);
long nugpgpu_fops_ioctl(struct file *nugpgpu_file,
                        unsigned int ioctl_num,
                        unsigned long ioctl_param);

static const struct file_operations
nugpgpu_file_ops = {
  .open = nugpgpu_fops_open,
  .unlocked_ioctl = nugpgpu_fops_ioctl
};

#endif
