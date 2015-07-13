#ifndef __NUGPGPU_GTT_H__
#define __NUGPGPU_GTT_H__

/* 
 * GRAPHICS TRANSLATION TABLE
 * https://bwidask.net/blog/index.php/2014/06/the-global-gtt-part-1/
 */

#include "nugpgpu_drv.h"
#include <linux/scatterlist.h>

int nugpgpu_gtt_init(struct nugpgpu_private *gpu_priv);
int nugpgpu_gtt_probe(struct nugpgpu_private *gpu_priv, size_t *total_size,
                  size_t *stolen_size, phys_addr_t *mappable_base,
                  unsigned long *mappable_end);
unsigned int nugpgpu_gtt_insert_entries(struct nugpgpu_private *gpu_priv,
                                        struct sg_table *st,
                                        unsigned int *entry,
                                        uint32_t level);

uint32_t nugpgpu_gtt_insert(struct nugpgpu_private *gpu_priv,
                            struct sg_table *st,
                            uint32_t level);

typedef uint32_t nugpgpu_gtt_pte_t;

#define PTE_VALID 1

/* PTE Structure
 * 31:12  Physical Page Address 31:12
 * 11     Cacheability Control (WT/WB LLC/eLLC(eDRAM)) ([3] of [3..0])
 * 10:4   Physical Page Address 38:32
 * 2:0    Cacheability Control (WT/WB LLC/eLLC(eDRAM)) ([2..0] of [3..0])
 * 0      Valid
 */
#define PTE_ADDR_ENCODE(addr, valid) \
          ((valid ? PTE_VALID : 0) | ((addr) | (((addr) >> 28) & 0x7f0)))

/* Cacheability Control
 * 3:1    Three lower bits
 * 11     Fourth bit
 */
#define HSW_CACHEABILITY_CONTROL(bits) \
          ((((bits) & 0x7) << 1) | \
           (((bits) & 0x8) << (11 - 3)))

#define PAGE_FAULT_REG 0x4090
#define PAGE_FAULT_BIT 14

#define GFX_FLSH_CNTL_HSW 0x101008
#define GFX_FLSH_CNTL_EN (1<<0)

#define IS_PAGE_FAULT_ENABLE  \
    ((NUGPGPU_READ(PAGE_FAULT_REG) >> PAGE_FAULT_BIT) & 1)


#endif
