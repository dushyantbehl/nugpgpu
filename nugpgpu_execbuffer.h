#ifndef __NUGPGPU_EXECBUFFER_H__
#define __NUGPGPU_EXECBUFFER_H__

#include "nugpgpu_drv.h"

int nugpgpu_execbuffer_init(void);
int nugpgpu_execbuffer_exec(struct nugpgpu_private *gpu_priv,
                            uint32_t ring_id);

#endif
