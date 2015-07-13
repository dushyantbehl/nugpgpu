#include "nugpgpu_ioctl.h"
#include "nuioctl.h"
#include <linux/device.h>

int nugpgpu_fops_init(struct nugpgpu_private *dev_priv)
{
  /*
   * http://lwn.net/Articles/49684
   */
  int chrdev;
  struct class *ret_err_class;
  struct device *ret_err_device;

  printk(LOG_INFO "nugpgpu_fops_init\n" LOG_END);
  TRACE_IN

  // Register character device
  chrdev = alloc_chrdev_region(&dev_priv->dev, 0, 1, "nugpgpu");
  if(chrdev < 0)
  {
    printk(LOG_ERR "alloc_chrdev_region failed\n" LOG_END);
    TRACE_OUT
    return chrdev;
  }
  printk(LOG_ERR "alloc_chrdev_region succeeded (major assigned: %d)\n"
          LOG_END, MAJOR(dev_priv->dev));

  cdev_init(&dev_priv->c_dev, &nugpgpu_file_ops);

  if((chrdev = cdev_add(&dev_priv->c_dev, dev_priv->dev, 1)) < 0)
  {
    printk(LOG_ERR "cdev_add failed\n" LOG_END);
    nugpgpu_fops_finish(dev_priv);
    TRACE_OUT
    return chrdev;
  }

  // Create device class
  if(IS_ERR(dev_priv->dev_class = class_create(THIS_MODULE, "nugpgpu")))
  {
    printk(LOG_ERR "class_create failed\n" LOG_END);
    ret_err_class = dev_priv->dev_class;
    nugpgpu_fops_finish(dev_priv);
    TRACE_OUT
    return PTR_ERR(ret_err_class);
  }
  printk(LOG_ERR "class_create succeeded\n" LOG_END);

  // Register device with sysfs
  if(IS_ERR(dev_priv->dev_device =
        device_create(dev_priv->dev_class, NULL, dev_priv->dev, NULL,
          "nugpgpu")))
  {
    printk(LOG_ERR "device_create failed\n" LOG_END);
    ret_err_device = dev_priv->dev_device;
    nugpgpu_fops_finish(dev_priv);
    TRACE_OUT
    return PTR_ERR(ret_err_device);
  }
  printk(LOG_ERR "device_create succeeded\n" LOG_END);

  TRACE_OUT
  return 0;
}

void nugpgpu_fops_finish(struct nugpgpu_private *dev_priv)
{
  printk(LOG_INFO "nugpgpu_fops_finish\n" LOG_END);
  TRACE_IN

  if(dev_priv->dev_device)
  {
    device_destroy(dev_priv->dev_class, dev_priv->dev);
    dev_priv->dev_device = 0;
  }

  if(dev_priv->dev_class)
  {
    class_destroy(dev_priv->dev_class);
    dev_priv->dev_class = 0;
  }

  cdev_del(&dev_priv->c_dev);

  if(dev_priv->dev)
  {
    unregister_chrdev_region(dev_priv->dev, 1);
    dev_priv->dev = 0;
  }

  TRACE_OUT
}

int nugpgpu_fops_open(struct inode *nugpgpu_inode,
                      struct file *nugpgpu_file)
{
  printk(LOG_INFO "nugpgpu_fops_open\n" LOG_END);
  TRACE_IN

  TRACE_OUT
  return 0;
}

long nugpgpu_fops_ioctl(struct file *nugpgpu_file,
                        unsigned int ioctl_num,
                        unsigned long ioctl_param)
{
  printk(LOG_INFO "nugpgpu_fops_ioctl 0x%x\n" LOG_END, ioctl_num);
  TRACE_IN

  switch (ioctl_num) {
  case NUIOCTL_EXM_NEW:
    printk(LOG_INFO "received ioctl : new\n" LOG_END);
    printk(LOG_INFO "new length : %d\n" LOG_END,
            ((struct nuioctl *) ioctl_param)->length);
    break;
  }

  TRACE_OUT
  return 0;
}
