#ifndef __NUGPGPU_VENDOR_H__
#define __NUGPGPU_VENDOR_H__

/*
 * nugpgpu: Vendor / Device Specific Information.
 * Taken directly from i915 driver source code.
 */

#include <linux/pci_ids.h>

/* TODO: Take out the relevant informations of both desktop
   and mobile gpu chipsets from this file to here because
   HOH will not have these headers */
#include <drm/i915_pciids.h>

#define NUGPGPU_VENDOR_ID   PCI_VENDOR_ID_INTEL
#define NUGPGPU_DEVICE_ID    
#define NUGPGPU_CLASS_ID   

#define ASSIGN_MAJOR 0
#define INTEL_MAJOR 1
#define INTEL_MINOR 6

/* TODO: This code should not be part of this file, move to a new file */
enum intel_ring_id {
    RCS = 0x0,
    VCS,
    BCS,
    VECS,
    VCS2
};

enum pipe {
    INVALID_PIPE = -1,
    PIPE_A = 0,
    PIPE_B,
    PIPE_C,
    _PIPE_EDP,
    I915_MAX_PIPES = _PIPE_EDP
};
#define pipe_name(p) ((p) + 'A')

enum transcoder {
    TRANSCODER_A = 0,
    TRANSCODER_B,
    TRANSCODER_C,
    TRANSCODER_EDP,
    I915_MAX_TRANSCODERS
};
#define transcoder_name(t) ((t) + 'A')

#define RENDER_RING     (1<<RCS)
#define BSD_RING        (1<<VCS)
#define BLT_RING        (1<<BCS)
#define VEBOX_RING      (1<<VECS)
#define BSD2_RING       (1<<VCS2)

/* Cursor Offsets */
#define CURSOR_A_OFFSET 0x70080
#define CURSOR_B_OFFSET 0x700c0
#define CHV_CURSOR_C_OFFSET 0x700e0
#define IVB_CURSOR_B_OFFSET 0x71080
#define IVB_CURSOR_C_OFFSET 0x72080

/* Pipe offsets */
#define PIPE_A_OFFSET       0x70000
#define PIPE_B_OFFSET       0x71000
#define PIPE_C_OFFSET       0x72000
#define CHV_PIPE_C_OFFSET   0x74000
/*
 * There's actually no pipe EDP. Some pipe registers have
 * simply shifted from the pipe to the transcoder, while
 * keeping their original offset. Thus we need PIPE_EDP_OFFSET
 * to access such registers in transcoder EDP.
 */
#define PIPE_EDP_OFFSET 0x7f000

/* Transcoder offsets */
#define TRANSCODER_A_OFFSET 0x60000
#define TRANSCODER_B_OFFSET 0x61000
#define TRANSCODER_C_OFFSET 0x62000
#define CHV_TRANSCODER_C_OFFSET 0x63000
#define TRANSCODER_EDP_OFFSET 0x6f000

/* Palette regs */
#define PALETTE_A_OFFSET 0xa000
#define PALETTE_B_OFFSET 0xa800
#define CHV_PALETTE_C_OFFSET 0xc000

#define DEV_INFO_FOR_EACH_FLAG(func, sep) \
    func(is_mobile) sep \
    func(is_i85x) sep \
    func(is_i915g) sep \
    func(is_i945gm) sep \
    func(is_g33) sep \
    func(need_gfx_hws) sep \
    func(is_g4x) sep \
    func(is_pineview) sep \
    func(is_broadwater) sep \
    func(is_crestline) sep \
    func(is_ivybridge) sep \
    func(is_valleyview) sep \
    func(is_haswell) sep \
    func(is_preliminary) sep \
    func(has_fbc) sep \
    func(has_pipe_cxsr) sep \
    func(has_hotplug) sep \
    func(cursor_needs_physical) sep \
    func(has_overlay) sep \
    func(overlay_needs_physical) sep \
    func(supports_tv) sep \
    func(has_llc) sep \
    func(has_ddi) sep \
    func(has_fpga_dbg)

#define DEFINE_FLAG(name) u8 name:1
#define SEP_SEMICOLON ;

struct intel_device_info {
    u32 display_mmio_offset;
    u8 num_pipes:3;
    u8 num_sprites[I915_MAX_PIPES];
    u8 gen;
    u8 ring_mask; /* Rings supported by the HW */
    DEV_INFO_FOR_EACH_FLAG(DEFINE_FLAG, SEP_SEMICOLON);
    /* Register offsets for the various display pipes and transcoders */
    int pipe_offsets[I915_MAX_TRANSCODERS];
    int trans_offsets[I915_MAX_TRANSCODERS];
    int palette_offsets[I915_MAX_PIPES];
    int cursor_offsets[I915_MAX_PIPES];
};

/* TODO: Remove this comment after moving the upper code to new files */
#define GEN_DEFAULT_PIPEOFFSETS \
    .pipe_offsets = { PIPE_A_OFFSET, PIPE_B_OFFSET, \
              PIPE_C_OFFSET, PIPE_EDP_OFFSET }, \
    .trans_offsets = { TRANSCODER_A_OFFSET, TRANSCODER_B_OFFSET, \
               TRANSCODER_C_OFFSET, TRANSCODER_EDP_OFFSET }, \
    .palette_offsets = { PALETTE_A_OFFSET, PALETTE_B_OFFSET }

#define IVB_CURSOR_OFFSETS \
    .cursor_offsets = { CURSOR_A_OFFSET, IVB_CURSOR_B_OFFSET, IVB_CURSOR_C_OFFSET }

#define GEN7_FEATURES                                   \
        .gen = 7, .num_pipes = 3, .need_gfx_hws = 1,    \
        .has_hotplug = 1, .has_fbc = 1,                 \
        .ring_mask = RENDER_RING | BSD_RING | BLT_RING, \
        .has_llc = 1

/* Device info for desktop based chipsets */
static const struct intel_device_info intel_haswell_d_info = {
    GEN7_FEATURES,
    .is_haswell = 1,
    .has_ddi = 1,
    .has_fpga_dbg = 1,
    .ring_mask = RENDER_RING | BSD_RING | BLT_RING | VEBOX_RING,
    GEN_DEFAULT_PIPEOFFSETS,
    IVB_CURSOR_OFFSETS,
};

/* Device info for mobile chipsets */
static const struct intel_device_info intel_haswell_m_info = {
    GEN7_FEATURES,
    .is_haswell = 1,
    .is_mobile = 1,
    .has_ddi = 1,
    .has_fpga_dbg = 1,
    .ring_mask = RENDER_RING | BSD_RING | BLT_RING | VEBOX_RING,
    GEN_DEFAULT_PIPEOFFSETS,
    IVB_CURSOR_OFFSETS,
};

#define NUGPGPU_PCI_IDLIST          \
    INTEL_HSW_D_IDS(&intel_haswell_d_info), \
    INTEL_HSW_M_IDS(&intel_haswell_m_info)

/* nugpgpu_vendor.h */

#endif
