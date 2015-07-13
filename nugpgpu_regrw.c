/*
 * nugpgpu: Register read / write code
 * Based on i915 driver source code.
 */
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/spinlock.h>

#include <asm-generic/io.h> // For readx and writex.

#include "nugpgpu_regrw.h"
#include "nugpgpu_drv.h"

#define NEEDS_FORCEWAKE(reg) \
        ((reg) < 0x40000 && (reg) != FORCEWAKE)

#define __raw_gpu_read8(nugpgpu_private__, reg__) readb((nugpgpu_private__)->regs + (reg__))
#define __raw_gpu_write8(nugpgpu_private__, reg__, val__) writeb(val__, (nugpgpu_private__)->regs + (reg__))

#define __raw_gpu_read16(nugpgpu_private__, reg__) readw((nugpgpu_private__)->regs + (reg__))
#define __raw_gpu_write16(nugpgpu_private__, reg__, val__) writew(val__, (nugpgpu_private__)->regs + (reg__))

#define __raw_gpu_read32(nugpgpu_private__, reg__) readl((nugpgpu_private__)->regs + (reg__))
#define __raw_gpu_write32(nugpgpu_private__, reg__, val__) writel(val__, (nugpgpu_private__)->regs + (reg__))

#define __raw_gpu_read64(nugpgpu_private__, reg__) readq((nugpgpu_private__)->regs + (reg__))
#define __raw_gpu_write64(nugpgpu_private__, reg__, val__) writeq(val__, (nugpgpu_private__)->regs + (reg__))

static void
gpu_unclaimed_reg_debug(struct nugpgpu_private *gpu_priv, uint32_t reg, bool read, bool before)
{
  const char *op = read ? "reading" : "writing to";
  const char *when = before ? "before" : "after";

  if(__raw_gpu_read32(gpu_priv, FPGA_DBG) & FPGA_DBG_RM_NOCLAIM) {
    printk(LOG_WARN "Unclaimed register called by nugpgpu %s %s register 0x%x\n" LOG_END, when, op, reg);
    __raw_gpu_write32(gpu_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);
  }
}

void forcewake_work(struct nugpgpu_private *gpu_priv)
{
    unsigned long irqflags;

    spin_lock_irqsave(&gpu_priv->mmio_funcs.lock, irqflags);
    if (--gpu_priv->forcewake_count == 0)
        forcewake_put(gpu_priv, 0 , true);
    spin_unlock_irqrestore(&gpu_priv->mmio_funcs.lock, irqflags);
}


/* 
 * Refer to this thread for information on forcewake shenanigans.
 * http://www.spinics.net/lists/intel-gfx/msg57987.html
 * */

void gpu_forcewake_get(struct nugpgpu_private *gpu_priv)
{
  unsigned long irqflags;
  TRACE_IN

  spin_lock_irqsave(&gpu_priv->mmio_funcs.lock, irqflags);
  if (gpu_priv->forcewake_count++ == 0){
    printk(LOG_INFO "going to call the forcewake get\n" LOG_END);
    forcewake_get(gpu_priv, 0, true);
  }
  spin_unlock_irqrestore(&gpu_priv->mmio_funcs.lock, irqflags);
  TRACE_OUT
}

void gpu_forcewake_put(struct nugpgpu_private *gpu_priv)
{
  unsigned long irqflags;
  TRACE_IN

  spin_lock_irqsave(&gpu_priv->mmio_funcs.lock, irqflags);
  if (--gpu_priv->forcewake_count == 0){
    printk(LOG_INFO "going to call the forcewake work\n" LOG_END);
    forcewake_put(gpu_priv, 0, true);
  }
  spin_unlock_irqrestore(&gpu_priv->mmio_funcs.lock, irqflags);
  TRACE_OUT
}

void forcewake_get(struct nugpgpu_private *gpu_priv, uint32_t reg, bool force)
{
  TRACE_IN
  if( force || NEEDS_FORCEWAKE(reg) ){

    if(wait_for_atomic((__raw_gpu_read32(gpu_priv, FORCEWAKE_ACK_HSW) & FORCEWAKE_KERNEL) 
                                         == 0, FORCEWAKE_ACK_TIMEOUT_MS))
      printk(LOG_ERR "Forcewake old clear timed out\n" LOG_END);

    __raw_gpu_write32(gpu_priv, FORCEWAKE_MT, _MASKED_BIT_ENABLE(FORCEWAKE_KERNEL));

    __raw_gpu_read32(gpu_priv, ECOBUS);

    if(wait_for_atomic((__raw_gpu_read32(gpu_priv, FORCEWAKE_ACK_HSW) & FORCEWAKE_KERNEL),
                        FORCEWAKE_ACK_TIMEOUT_MS))
      printk(LOG_ERR "Forcewake new clear timed out\n" LOG_END);

    if(wait_for_atomic_us((__raw_gpu_read32(gpu_priv, GEN6_GT_THREAD_STATUS_REG) &
                                                      GEN6_GT_THREAD_STATUS_CORE_MASK_HSW) 
                                                      == 0, 500))
      printk(LOG_ERR "GT thread waiting status timed out\n" LOG_END);
    }
  TRACE_OUT
}

void forcewake_put(struct nugpgpu_private *gpu_priv, uint32_t reg, bool force)
{
  uint32_t gtfifodbg;
  TRACE_IN
  if( force || NEEDS_FORCEWAKE(reg) ){
    __raw_gpu_write32(gpu_priv, FORCEWAKE_MT, _MASKED_BIT_DISABLE(FORCEWAKE_KERNEL));
    __raw_gpu_read32(gpu_priv, ECOBUS);

    gtfifodbg = __raw_gpu_read32(gpu_priv, GTFIFODBG);
    if ( gtfifodbg & GT_FIFO_CPU_ERROR_MASK ){
        printk(LOG_WARN "MMIO read/write has been dropped GTFIFODBG - %x\n" LOG_END, gtfifodbg);
        __raw_gpu_write32(gpu_priv, GTFIFODBG, GT_FIFO_CPU_ERROR_MASK);
    }
  }
  TRACE_OUT
}

#define __gpu_read(x) \
static u##x \
gpu_read##x(struct nugpgpu_private *gpu_priv, off_t reg) {\
  unsigned long irqflags; \
  u##x val = 0; \
  TRACE_IN \
  spin_lock_irqsave(&gpu_priv->mmio_funcs.lock, irqflags); \
  if (gpu_priv->forcewake_count == 0) \
    forcewake_get(gpu_priv, reg, false); \
  val = __raw_gpu_read##x(gpu_priv, reg); \
  if (gpu_priv->forcewake_count == 0) \
    forcewake_put(gpu_priv, reg, false); \
  spin_unlock_irqrestore(&gpu_priv->mmio_funcs.lock, irqflags); \
  PRINT_REG(reg,val); \
  TRACE_OUT \
  return val; \
}

#define __gpu_write(x) \
static void \
gpu_write##x(struct nugpgpu_private *gpu_priv, off_t reg, u##x val) {\
  unsigned long irqflags; \
  TRACE_IN \
  spin_lock_irqsave(&gpu_priv->mmio_funcs.lock, irqflags); \
  gpu_unclaimed_reg_debug(gpu_priv, reg, false, true); \
  __raw_gpu_write##x(gpu_priv, reg, val); \
  gpu_unclaimed_reg_debug(gpu_priv, reg, false, false); \
  spin_unlock_irqrestore(&gpu_priv->mmio_funcs.lock, irqflags); \
  PRINT_REG(reg, val)\
  TRACE_OUT \
}

__gpu_read(8)
__gpu_read(16)
__gpu_read(32)
__gpu_read(64)

__gpu_write(8)
__gpu_write(16)
__gpu_write(32)
__gpu_write(64)


void nugpgpu_regrw_init(struct nugpgpu_private *gpu_priv)
{
  printk(LOG_INFO "nugpgpu_regrw_init\n" LOG_END);
  TRACE_IN

  nugpgpu_uncore_early_sanitize(gpu_priv);

  spin_lock_init(&gpu_priv->mmio_funcs.lock);

  gpu_priv->mmio_funcs.mmio_readb = gpu_read8;
  gpu_priv->mmio_funcs.mmio_readw = gpu_read16;
  gpu_priv->mmio_funcs.mmio_readl = gpu_read32;
  gpu_priv->mmio_funcs.mmio_readq = gpu_read64;

  gpu_priv->mmio_funcs.mmio_writeb = gpu_write8;
  gpu_priv->mmio_funcs.mmio_writew = gpu_write16;
  gpu_priv->mmio_funcs.mmio_writel = gpu_write32;
  gpu_priv->mmio_funcs.mmio_writeq = gpu_write64;

  // nugpgpu_regrw_sanitize(gpu_priv);

  TRACE_OUT
}

void nugpgpu_uncore_early_sanitize(struct nugpgpu_private *gpu_priv)
{
  printk(LOG_INFO "nugpgpu_uncore_early_sanitize\n" LOG_END);
  TRACE_IN
    //TODO: Looks like not needed so remove.
  
  __raw_gpu_write32(gpu_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);
 
  //force_wake_reset
  __raw_gpu_write32(gpu_priv, FORCEWAKE, 0);
  //Read something from the same cacheline but !FORCEWAKE.
  __raw_gpu_read32(gpu_priv, ECOBUS);


  //force_wake_mt_reset.
  __raw_gpu_write32(gpu_priv, FORCEWAKE_MT, _MASKED_BIT_DISABLE(0xffff));
  //Same cacheline logic as above.
  __raw_gpu_read32(gpu_priv, ECOBUS);

  TRACE_OUT
  return;
}


void nugpgpu_regrw_sanitize(struct nugpgpu_private *gpu_priv)
{
  printk(LOG_INFO "nugpgpu_regrw_sanitize\n" LOG_END);
  TRACE_IN

  printk(LOG_INFO "FPGA_DBG: %x\n" LOG_END, NUGPGPU_READ(FPGA_DBG));
  printk(LOG_INFO "GTFIFODBG: %x\n" LOG_END, NUGPGPU_READ(GTFIFODBG));
  printk(LOG_INFO "HSW_ERR_INT: %x\n" LOG_END, NUGPGPU_READ(HSW_ERR_INT));

  NUGPGPU_WRITE(FPGA_DBG, NUGPGPU_READ(FPGA_DBG));
  NUGPGPU_WRITE(GTFIFODBG, NUGPGPU_READ(GTFIFODBG));
  NUGPGPU_WRITE(HSW_ERR_INT, NUGPGPU_READ(HSW_ERR_INT));

  printk(LOG_INFO "FPGA_DBG: %x\n" LOG_END, NUGPGPU_READ(FPGA_DBG));
  printk(LOG_INFO "GTFIFODBG: %x\n" LOG_END, NUGPGPU_READ(GTFIFODBG));
  printk(LOG_INFO "HSW_ERR_INT: %x\n" LOG_END, NUGPGPU_READ(HSW_ERR_INT));

  TRACE_OUT
}
