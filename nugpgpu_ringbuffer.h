#ifndef __NUGPGPU_RINGBUFFER_H__
#define __NUGPGPU_RINGBUFFER_H__

#include "nugpgpu_drv.h"
#include "nugpgpu_memmgr.h"

#define RENDER_HWS_PGA_GEN7 (0x04080)

#define RENDER_RING_BASE  0x02000

#define RING_NR_PAGES   0x001FF000
#define RING_HEAD_ADDR  0x001FFFFC
#define RING_TAIL_ADDR  0x001FFFF8
#define RING_FREE_SPACE 64

#define RING_TAIL(base)     ((base)+0x30)
#define RING_HEAD(base)     ((base)+0x34)
#define RING_START(base)    ((base)+0x38)
#define RING_CTL(base)      ((base)+0x3c)
#define RING_MI_MODE(base)  ((base)+0x9c)
#define RING_INSTPM(base)   ((base)+0xc0)
#define RING_ACTHD(base)    ((base)+0x74)
#define RING_INSTDONE(base) ((base)+0x6c)
#define RING_MODE_GEN7(base) ((base)+0x29c)

#define RING_READ_TAIL(ring)    \
  NUGPGPU_READ(RING_TAIL((ring)->mmio_base))

#define RING_READ_HEAD(ring)    \
  NUGPGPU_READ(RING_HEAD((ring)->mmio_base))

#define RING_READ_START(ring)   \
  NUGPGPU_READ(RING_START((ring)->mmio_base))

#define RING_READ_CTL(ring)     \
  NUGPGPU_READ(RING_CTL((ring)->mmio_base))

#define RING_READ_MODE(ring)    \
  NUGPGPU_READ(RING_MI_MODE((ring)->mmio_base))

#define RING_READ_INSTPM(ring)  \
  NUGPGPU_READ(RING_INSTPM((ring)->mmio_base))

#define RING_READ_ACTHD(ring)   \
  NUGPGPU_READ(RING_ACTHD((ring)->mmio_base))

#define RING_READ_INSTDONE(ring)     \
  NUGPGPU_READ(RING_ISNTDONE((ring)->mmio_base))


#define RING_WRITE_TAIL(ring, val)  \
  NUGPGPU_WRITE(RING_TAIL((ring)->mmio_base), val)

#define RING_WRITE_HEAD(ring, val)  \
  NUGPGPU_WRITE(RING_HEAD((ring)->mmio_base), val)

#define RING_WRITE_START(ring, val) \
  NUGPGPU_WRITE(RING_START((ring)->mmio_base), val)

#define RING_WRITE_CTL(ring, val)   \
  NUGPGPU_WRITE(RING_CTL((ring)->mmio_base), val)

#define RING_WRITE_MODE(ring, val)   \
  NUGPGPU_WRITE(RING_MI_MODE((ring)->mmio_base), val)

#define RING_WRITE_INSTPM(ring, val)   \
  NUGPGPU_WRITE(RING_INSTPM((ring)->mmio_base), val)

#define RING_WRITE_MODE_GEN7(ring, val)   \
  NUGPGPU_WRITE(RING_MODE_GEN7((ring)->mmio_base), val)

#define STOP_RING               (1<<8)
#define MODE_IDLE               (1<<9)
#define VS_TIMER_DISPATCH       (1<<6)
#define ASYNC_FLIP_PERF_DISABLE (1<<14)
#define INSTPM_TLB_INVALIDATE   (1<<9)
#define INSTPM_SYNC_FLUSH       (1<<5)
#define INSTPM_FORCE_ORDERING   (1<<7)
#define RING_VALID              0x1
#define SEQ_NO_INDEX            0x20
#define MI_PREDICATE_RESULT_2   0x2214
#define LOWER_SLICE_DISABLED    (0<<0)
#define HSW_IDICR               0x9008
#define IDIHASHMSK(x)           (((x) & 0x3f) << 16)

#define GFX_TLB_INVALIDATE_ALWAYS  (1<<13)
#define GFX_REPLAY_MODE (1<<11)

/*
 * Memory interface instructions for ring.
 */
#define MI_INSTR(opcode, flags) (((opcode) << 23) | (flags))
#define MI_FLUSH    MI_INSTR(0x04, 0)
#define MI_STORE_DWORD_IMM MI_INSTR(0x20, 1)

#define MI_STORE_DWORD_INDEX MI_INSTR(0x21, 1)
#define MI_STORE_DWORD_INDEX_SHIFT 2
#define MI_USER_INTERRUPT MI_INSTR(0x02, 0)
#define MI_NOOP MI_INSTR(0, 0)

#define MI_BATCH_BUFFER_START MI_INSTR(0x31, 0)
#define MI_BATCH_BUFFER_END   MI_INSTR(0x0a, 0)

struct nugpgpu_ring {
  struct nugpgpu_private *gpu_priv;
  u32 mmio_base;
  int size;                     // Ring-buffer size
  void __iomem *virtual_start;
  u32 *page_ringbuffer;         // Kernel VA ring-buffer page
  u32 *page_status;             // Kernel VA status page
  u32 gva_ringbuffer;           // Graphics virtual address ring-buffer
  unsigned int gva_status;      // Graphics virtual address status page
  struct nugpgpu_obj status_obj;
  struct nugpgpu_obj ringbuf_obj;
  u32 head;
  u32 tail;
  int space;
  int effective_size;
};

int nugpgpu_ringbuffer_render_init(struct nugpgpu_private *gpu_priv);
inline int ring_space(struct nugpgpu_ring *ring);
void nugpgpu_ring_advance(struct nugpgpu_private *gpu_priv,
                          struct nugpgpu_ring *ring);
inline void ring_emit(struct nugpgpu_ring *ring, u32 data);
int nugpgpu_ring_begin(struct nugpgpu_ring *ring, int num_dwords);

#endif
