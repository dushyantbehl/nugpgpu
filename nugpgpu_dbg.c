#include "nugpgpu_dbg.h"
#include "linux/pci.h"
#include "linux/device.h"

int depth = 0;
int debug = 1;

static void my915_pcie_dump(struct nugpgpu_private *gpu_priv)
{
  struct pci_dev *dev = gpu_priv->pdev;

  dev_info(&dev->dev, "\n\nPCIE DUMP\n\n");

  #define MY915_READ NUGPGPU_READ

  #define GTTMMADR_DPFC_CONTROL 0x100100
  #define GTTMMADR_MTOLUD 0x108000
  #define GTTMMADR_MGGC 0x108040
  #define GTTMMADR_MTOUUD 0x108080
  #define GTTMMADR_MBDSM 0x1080c0
  #define GTTMMADR_MBGSM 0x108100
  #define GTTMMADR_MEMRR_BASE 0x108340
  dev_info(&dev->dev, "GTTMMADR_DPFC_CONTROL\t0x%x\n", MY915_READ(GTTMMADR_DPFC_CONTROL));
  dev_info(&dev->dev, "GTTMMADR_MTOLUD\t\t0x%x\n", MY915_READ(GTTMMADR_MTOLUD));
  dev_info(&dev->dev, "GTTMMADR_MGGC\t\t0x%x\n", MY915_READ(GTTMMADR_MGGC));
  dev_info(&dev->dev, "GTTMMADR_MTOUUD\t\t0x%x\n", MY915_READ(GTTMMADR_MTOUUD));
  dev_info(&dev->dev, "GTTMMADR_MBDSM\t\t0x%x\n", MY915_READ(GTTMMADR_MBDSM));
  dev_info(&dev->dev, "GTTMMADR_MBGSM\t\t0x%x\n", MY915_READ(GTTMMADR_MBGSM));
  dev_info(&dev->dev, "GTTMMADR_MEMRR_BASE\t\t0x%x\n", MY915_READ(GTTMMADR_MEMRR_BASE));

}

static void my915_mmio_dump(struct nugpgpu_private *gpu_priv)
{
  struct pci_dev *dev = gpu_priv->pdev;

  dev_info(&dev->dev, "\n\nMMIO DUMP\n\n");

  //GT POWERSAVE
  #define MMIO_GEN6_RC_CONTROL 0xA090
  #define MMIO_GEN6_RPNSWREQ 0xA008
  #define MMIO_GEN6_PMINTRMSK 0xA168
  #define MMIO_GEN6_PMIER 0x4402C
  #define MMIO_GEN6_PMIIR 0x44028
  // FORCEWAKE
  #define MMIO_ECOBUS 0xA180
  #define MMIO_FORCEWAKE 0xA18C
  #define MMIO_FORCEWAKE_MT 0xA188

  #define MMIO_MI_PREDICATE_RESULT_2 0x2214
  #define MMIO_HSW_IDICR 0x9008
  // RING
#ifndef RENDER_RING_BASE
  #define RENDER_RING_BASE  0x02000
  #define RENDER_RING_MMIO_DEBUG 0x04080

  #define RING_TAIL(base)     ((base)+0x30)
  #define RING_HEAD(base)     ((base)+0x34)
  #define RING_START(base)    ((base)+0x38)
  #define RING_CTL(base)      ((base)+0x3c)
  #define RING_MI_MODE(base)  ((base)+0x9c)
  #define RING_ACTHD(base)    ((base)+0x74)
  #define RING_INSTDONE(base) ((base)+0x6c)


  #define RING_READ_TAIL(ring)  MY915_READ(RING_TAIL((ring)))
  #define RING_READ_HEAD(ring)  MY915_READ(RING_HEAD((ring)))
  #define RING_READ_START(ring) MY915_READ(RING_START((ring)))
  #define RING_READ_CTL(ring)   MY915_READ(RING_CTL((ring)))
  #define RING_READ_MODE(ring)  MY915_READ(RING_MI_MODE((ring)))
  #define RING_READ_ACTHD(ring)      MY915_READ(RING_ACTHD((ring)))
  #define RING_READ_INSTDONE(ring)      MY915_READ(RING_INSTDONE((ring)))
#endif
  dev_info(&dev->dev, "MMIO_GEN6_RC_CONTROL\t\t0x%x\n", MY915_READ(MMIO_GEN6_RC_CONTROL));
  dev_info(&dev->dev, "MMIO_GEN6_RPNSWREQ\t\t0x%x\n", MY915_READ(MMIO_GEN6_RPNSWREQ));
  dev_info(&dev->dev, "MMIO_GEN6_PMINTRMSK\t\t0x%x\n", MY915_READ(MMIO_GEN6_PMINTRMSK));
  dev_info(&dev->dev, "MMIO_GEN6_PMIER\t\t0x%x\n", MY915_READ(MMIO_GEN6_PMIER));
  dev_info(&dev->dev, "MMIO_GEN6_PMIIR\t\t0x%x\n", MY915_READ(MMIO_GEN6_PMIIR));
  dev_info(&dev->dev, "MMIO_ECOBUS\t\t0x%x\n", MY915_READ(MMIO_ECOBUS));
  dev_info(&dev->dev, "MMIO_FORCEWAKE\t\t0x%x\n", MY915_READ(MMIO_FORCEWAKE));
  dev_info(&dev->dev, "MMIO_FORCEWAKE_MT\t\t0x%x\n", MY915_READ(MMIO_FORCEWAKE_MT));
  dev_info(&dev->dev, "MMIO_MI_PREDICATE_RESULT_2\t\t0x%x\n", MY915_READ(MMIO_MI_PREDICATE_RESULT_2));
  dev_info(&dev->dev, "MMIO_HSW_IDICR\t\t0x%x\n", MY915_READ(MMIO_HSW_IDICR));
  dev_info(&dev->dev, "RENDER_RING_MMIO\t\t0x%x\n", MY915_READ(RENDER_RING_MMIO_DEBUG));
  dev_info(&dev->dev, "RING_TAIL\t\t\t0x%x\n", RING_READ_TAIL(RENDER_RING_BASE));
  dev_info(&dev->dev, "RING_HEAD\t\t\t0x%x\n", RING_READ_HEAD(RENDER_RING_BASE));
  dev_info(&dev->dev, "RING_START\t\t\t0x%x\n", RING_READ_START(RENDER_RING_BASE));
  dev_info(&dev->dev, "RING_CTL\t\t\t0x%x\n", RING_READ_CTL(RENDER_RING_BASE));
  dev_info(&dev->dev, "RING_MODE\t\t\t0x%x\n", RING_READ_MODE(RENDER_RING_BASE));
  dev_info(&dev->dev, "RING_ACTHD\t\t\t0x%x\n", RING_READ_ACTHD(RENDER_RING_BASE));
  dev_info(&dev->dev, "RING_INSTDONE\t\t\t0x%x\n", RING_READ_INSTDONE(RENDER_RING_BASE));

}

void nugpgpu_dump(struct nugpgpu_private *gpu_priv)
{
  debug = 0;
  dev_info(&gpu_priv->pdev->dev, "\n");
  my915_pcie_dump(gpu_priv);
  my915_mmio_dump(gpu_priv);
  dev_info(&gpu_priv->pdev->dev, "\n");
  debug = 1;
}
