#ifndef __NUGPGPU_PCIOFFSET_H__
#define __NUGPGPU_PCIOFFSET_H__

#define HSW_GMCH_CTRL 0x50

// GTT Graphics Memory Size
#define HSW_GMCH_GGMS_SHIFT 8
#define HSW_GMCH_GGMS_REVSHIFT 20
#define HSW_GMCH_GGMS_MASK 0x3

// Graphics Mode Select
#define HSW_GMCH_GMS_SHIFT 3
#define HSW_GMCH_GMS_REVSHIFT 25
#define HSW_GMCH_GMS_MASK 0x1f

#endif
