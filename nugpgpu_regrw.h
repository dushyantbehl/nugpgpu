/*
 * nugpgpu: Register read / write code
 * Based on i915 driver source code.
 */
#ifndef __NUGPGPU_REGRW_H__
#define __NUGPGPU_REGRW_H__

#include <linux/spinlock_types.h>

// TODO : Find reference. Not in the docs.
#define FPGA_DBG 0x42300
#define FPGA_DBG_RM_NOCLAIM (1<<31)
#define GTFIFODBG 0x120000
#define GT_FIFO_CPU_ERROR_MASK 7
#define HSW_ERR_INT 0x44040

#define ECOBUS 0xA180
#define FORCEWAKE_MT 0xA188
#define FORCEWAKE 0xA18C
#define FORCEWAKE_ACK_HSW 0x130044
#define FORCEWAKE_KERNEL 0x1
#define FORCEWAKE_ACK_TIMEOUT_MS 2

#define GEN6_GT_THREAD_STATUS_REG 0x13805c
#define GEN6_GT_THREAD_STATUS_CORE_MASK_HSW (0x7 | (0x07 << 16))

#define _MASKED_BIT_ENABLE(a)   (((a) << 16) | (a))
#define _MASKED_BIT_DISABLE(a)  ((a) << 16)

struct nugpgpu_private;

struct gpu_mmio_funcs {
  spinlock_t lock;

  uint8_t (*mmio_readb)(struct nugpgpu_private *gpu_priv, off_t offset);
  uint16_t (*mmio_readw)(struct nugpgpu_private *gpu_priv, off_t offset);
  uint32_t (*mmio_readl)(struct nugpgpu_private *gpu_priv, off_t offset);
  uint64_t (*mmio_readq)(struct nugpgpu_private *gpu_priv, off_t offset);

  void (*mmio_writeb)(struct nugpgpu_private *gpu_priv, off_t offset,
                      uint8_t val);
  void (*mmio_writew)(struct nugpgpu_private *gpu_priv, off_t offset,
                      uint16_t val);
  void (*mmio_writel)(struct nugpgpu_private *gpu_priv, off_t offset,
                      uint32_t val);
  void (*mmio_writeq)(struct nugpgpu_private *gpu_priv, off_t offset,
                      uint64_t val);
};

void nugpgpu_regrw_init(struct nugpgpu_private *gpu_priv);
void nugpgpu_regrw_sanitize(struct nugpgpu_private *gpu_priv);
void nugpgpu_uncore_early_sanitize(struct nugpgpu_private *gpu_priv);

void gpu_forcewake_get(struct nugpgpu_private *gpu_priv);
void gpu_forcewake_put(struct nugpgpu_private *gpu_priv);

void forcewake_get(struct nugpgpu_private *gpu_priv, uint32_t reg, bool force);
void forcewake_put(struct nugpgpu_private *gpu_priv, uint32_t reg, bool force);

#define NUGPGPU_READ(reg)       \
          gpu_priv->mmio_funcs.mmio_readl(gpu_priv, (reg))
#define NUGPGPU_WRITE(reg, val) \
          gpu_priv->mmio_funcs.mmio_writel(gpu_priv, (reg), (val))

#define NUGPGPU_READQ(reg)       \
          gpu_priv->mmio_funcs.mmio_readq(gpu_priv, (reg))
#define NUGPGPU_WRITEQ(reg, val) \
          gpu_priv->mmio_funcs.mmio_writeq(gpu_priv, (reg), (val))
#endif
