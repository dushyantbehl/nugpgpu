#ifndef __NUGPGPU_DRV_H__
#define __NUGPGPU_DRV_H__

#include <linux/types.h>
#include <linux/cdev.h>

#include "nugpgpu_regrw.h"

/*
 * TODO : Find a way to move this to nugpgpu_gtt.h
 *
 * GTT : GPU's VA -> PA
 * GTT logic in GPU, and not iommu.
 * Each element in GTT is a PTE.
 * Most of the initialization done by firmware.
 * The required information from initialization process can be obtained via PCI
 * config space, or MMIO. (We do it by PCI config space).
 * GPU mapping is direct, hardware based.
 * CPU needs to get the mapping, and then dereference & write.
 *
 * GSM : Graphics Stolen Memory, holds global PTEs
 * Located in system memory, allocated by BIOS, or boot firmware.
 * Base address from MPGFXTRK_CR_MBGSM_0_2_0_GTTMMADR (0x108100) (31:20)
 *
 * DSM : Stolen memory for everything that is not GTT.
 *
 * UNCORE_CR_GTTMMADR_0_2_0_PCI (0x10) (38:22)
 * Requests allocation for the combined GTT aperture and memory mapped range.
 * 4MB = 2MB MMIO + 2MB GTT aperture.
 * Base address of GTTADR := 2MB +  GTTMMADR
 * Base address of MMIO := GTTMMADR
 * Entire 4MB range is defined as a memory BAR in graphics device config space.
 * Alias into which the driver is required to write PTEs.
 * TODO : VERIFY: WHY: HOW: Driver can read PTE from GTT, but cannot write
 * directly to GTT. (To enable GPU TLB update?)
 *
 * Some region is mapped by the CPU too - GMADR (=aperture?)
 *
 * start : Start offset (0)
 *
 * total : Size of address space mapping (2GB)
 *
 * scratchpage : All unused PTEs point to this blank page
 */

struct gpu_gtt {
  unsigned long mappable_end;
  struct io_mapping *mappable;
  phys_addr_t mappable_base;

  unsigned long start;

  size_t stolen_size;
  size_t total_size;

  void __iomem *gsm;

  struct {
    dma_addr_t addr;
    struct page *page;
  } scratchpage;
};

// Stripped down version of `struct drm_i915_private`
struct nugpgpu_private {
  struct pci_dev *pdev;
  struct pci_dev *bridge_dev;

  void __iomem *regs;
  struct gpu_gtt gtt;

  struct gpu_mmio_funcs mmio_funcs;
  unsigned forcewake_count;

  dev_t dev;

  struct cdev c_dev;
  struct class *dev_class;
  struct device *dev_device;
};

#define NUGPGPU_CACHE_LLC 1

#include "nugpgpu_dbg.h"

/* Taken from i915/intel_drv.h */
#include <linux/delay.h>
/**
 * _wait_for - magic (register) wait macro
 *
 * Does the right thing for modeset paths when run under kdgb or similar atomic
 * contexts. Note that it's important that we check the condition again after
 * having timed out, since the timeout could be due to preemption or similar and
 * we've never had a chance to check the condition before the timeout.
 **/
#define _wait_for(COND, MS, W) ({ \
  unsigned long timeout__ = jiffies + msecs_to_jiffies(MS) + 1;   \
  int ret__ = 0;                                                  \
  while (!(COND)) {                                               \
          if (time_after(jiffies, timeout__)) {                   \
                  if (!(COND))                                    \
                          ret__ = -ETIMEDOUT;                     \
                  break;                                          \
          }                                                       \
          if (W)  {                                               \
                  msleep(W);                                      \
          } else {                                                \
                  cpu_relax();                                    \
          }                                                       \
  }                                                               \
  ret__;                                                          \
})

#define wait_for(COND, MS) _wait_for(COND, MS, 1)
#define wait_for_atomic(COND, MS) _wait_for(COND, MS, 0)
#define wait_for_atomic_us(COND, US) _wait_for((COND), DIV_ROUND_UP((US), 1000), 0)

#endif
