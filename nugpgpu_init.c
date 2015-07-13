/*
 * nugpgpu: Driver initialization and exit routines
 * Based on i915 driver source code.
 */

#include <linux/module.h>
#include <linux/pci.h>

#include "nugpgpu_drv.h" /* Driver core structs */
#include "nugpgpu_vendor.h" /* Device specific details. */
#include "nugpgpu_regrw.h" /* Device register reaq/write handles */
#include "nugpgpu_dbg.h" /* Driver debug module */
#include "nugpgpu_gtt.h" /* Device gtt */
#include "nugpgpu_ioctl.h" /* Driver ioctls */
#include "nugpgpu_execbuffer.h" /* Execbuffer module */
#include "nugpgpu_ringbuffer.h" /* Device ringbuffer handler */

#define DRIVER_LICENSE      "GPL and additional rights"
#define DRIVER_NAME         "nugpgpu"
#define DRIVER_DESCRIPTION  "New GPGPU Driver for Intel Haswell Chipsets"
#define DRIVER_AUTHOR       "Dushyant Behl <myselfdushyantbehl@gmail.com, "\
                            "Utkarsh <utkarshsins@gmail.com>"

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);

static int __init nugpgpu_init (void);
static void nugpgpu_hw_init(struct nugpgpu_private *gpu_priv);

/* Array of pci device id's we support. */
static const struct pci_device_id nugpgpu_id_list[] = {
  NUGPGPU_PCI_IDLIST,
  {0, 0, 0}      /* No Idea currently what these zeros are, padding maybe */
};
MODULE_DEVICE_TABLE(pci, nugpgpu_id_list);

static int nugpgpu_mmio_map(struct pci_dev *pdev,
                            struct nugpgpu_private * dev_priv)
{
  int mmio_bar;
  int mmio_size;

  printk(LOG_INFO "nugpgpu_mmio_map\n" LOG_END);
  TRACE_IN

  mmio_bar = 0;
  mmio_size = 2*1024*1024;

  dev_priv->regs = pci_iomap(pdev, mmio_bar, mmio_size);
  if (!dev_priv->regs)
  {
    printk(LOG_ERR "Failed to map registers\n" LOG_END);
    return -EIO;
  }

  printk(LOG_INFO "MMIO Bar : %d\n" LOG_END, mmio_bar);
  printk(LOG_INFO "MMIO Size : 0x%x\n" LOG_END, mmio_size);
  printk(LOG_INFO "MMIO registers successfully mapped to 0x%lx\n" LOG_END,
        (unsigned long) dev_priv->regs);

  TRACE_OUT
  return 0;
}

static int nugpgpu_pci_probe(struct pci_dev *pdev,
                              const struct pci_device_id *ent)
{
  int ret;
  struct nugpgpu_private * dev_priv;

  printk(LOG_INFO "nugpgpu_pci_probe\n" LOG_END);
  TRACE_IN

  printk(LOG_INFO "Got information for a device - %s \n" LOG_END,
          pci_name(pdev));

  dev_priv = kzalloc(sizeof(*dev_priv), GFP_KERNEL);
  if (dev_priv == NULL)
  {
    printk(LOG_ERR "Failed to allocate memory for dev_priv\n" LOG_END);
    TRACE_OUT
    return -ENOMEM;
  }

  ret = pci_enable_device(pdev);
  if (ret)
  {
    printk(LOG_ERR "pci_enable_device returned %d \n" LOG_END, ret);
    // TODO : free dev_priv
    TRACE_OUT
    return ret;
  }

  dev_priv->pdev = pdev;
  pci_set_drvdata(pdev, dev_priv);

  ret = nugpgpu_fops_init(dev_priv);
  if (ret)
  {
    printk(LOG_INFO "nugpgpu_fops_init returned %d \n" LOG_END, ret);
    // TODO : free drv_priv, pci_disable_device
    TRACE_OUT
    return ret;
  }

  //TODO: Don't know why it is critical to i915 yet.
  dev_priv->bridge_dev = pci_get_bus_and_slot(0, PCI_DEVFN(0,0)); 

  nugpgpu_mmio_map(pdev, dev_priv);

  nugpgpu_regrw_init(dev_priv);

  nugpgpu_gtt_init(dev_priv);

  pci_set_master(pdev);

  nugpgpu_hw_init(dev_priv);
  nugpgpu_ringbuffer_render_init(dev_priv);

  TRACE_OUT
  return 0;
}

static void nugpgpu_hw_init(struct nugpgpu_private *gpu_priv)
{
  printk(LOG_INFO "nugpgpu_hw_init\n" LOG_END);
  TRACE_IN

  // TODO: Not needed. Default works with GT2 Haswell.
  #define MI_PREDICATE_RESULT_2 0x2214 // HSW-Command-Reference-Registers
  printk(LOG_INFO "MI_PREDICATE_RESULT_2 0x%x\n" LOG_END,
          NUGPGPU_READ(MI_PREDICATE_RESULT_2));
  NUGPGPU_WRITE(MI_PREDICATE_RESULT_2, 0);
  printk(LOG_INFO "MI_PREDICATE_RESULT_2 0x%x\n" LOG_END,
          NUGPGPU_READ(MI_PREDICATE_RESULT_2));

  TRACE_OUT
}

static void nugpgpu_pci_remove(struct pci_dev *pdev)
{
  struct nugpgpu_private * dev_priv = pci_get_drvdata(pdev);

  printk(LOG_INFO "nugpgpu_pci_remove\n" LOG_END);
  TRACE_IN

  nugpgpu_fops_finish(dev_priv);

  if (dev_priv->regs != NULL)
    pci_iounmap(pdev, dev_priv->regs);

  pci_disable_device(pdev);

  TRACE_OUT
  return;
}

/* struct PCI_DRIVER to register the pci driver with the kernel. */
static struct pci_driver nugpgpu_pci_driver = {
  name: DRIVER_NAME,
  id_table: nugpgpu_id_list,
  probe: nugpgpu_pci_probe,
  remove: nugpgpu_pci_remove,
};

/* Driver Initialization Routine. */
static int __init nugpgpu_init (void)
{
  int rc;

  TRACE_INIT

  printk(LOG_INFO "nugpgpu_init\n" LOG_END);
  TRACE_IN

  printk(LOG_INFO "Initializing module\n" LOG_END);

  rc = pci_register_driver( &nugpgpu_pci_driver );
  if ( rc == 0 )
      printk(LOG_INFO "Driver registered successfully\n" LOG_END);
  else
      printk(LOG_ERR "Driver registeration failed\n" LOG_END);

  TRACE_OUT
  return rc;
}

/* Driver Exit Routine. */
static void __exit nugpgpu_exit (void)
{
  printk(LOG_INFO "nugpgpu_exit\n" LOG_END);
  TRACE_IN

  printk(LOG_INFO "Exiting module\n" LOG_END);

  pci_unregister_driver(&nugpgpu_pci_driver);
  printk(LOG_INFO "Driver unregistered successfully\n" LOG_END);
  TRACE_OUT
}

module_init(nugpgpu_init);
module_exit(nugpgpu_exit);

/* nugpgpu_init.c */
