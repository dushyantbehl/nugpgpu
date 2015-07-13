#include <linux/pci.h>

#include "nugpgpu_execbuffer.h"
#include "nugpgpu_gtt.h"

char nugpgpu_buff[4096] __attribute__((aligned(4096)));

int nugpgpu_execbuffer_init(void)
{
  printk(LOG_INFO "nugpgpu_execbuffer_init\n" LOG_END);
  TRACE_IN

  printk(LOG_INFO "buff : 0x%lx" LOG_END,
        (long unsigned int) (nugpgpu_buff));
  printk(LOG_INFO "buff-paddr : 0x%lx\n" LOG_END,
        (long unsigned int) (virt_to_phys(nugpgpu_buff)));


  TRACE_OUT
  return 0;
}

int nugpgpu_execbuffer_exec(struct nugpgpu_private *gpu_priv,
                            uint32_t ring_id)
{
  printk(LOG_INFO "nugpgpu_execbuffer_exec\n" LOG_END);
  TRACE_IN

  TRACE_OUT
  return 0;
}
