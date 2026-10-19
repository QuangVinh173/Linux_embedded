#include <linux/module.h>
#include <linux/fs.h> // alloc_chrdev_region
#include <linux/pci.h> // ioremap
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/atomic.h>
#include "usb1.h"
#include "main.h"
#include "cdc_ioctl.h"

#define DRIVER_NAME "usb1_driver"
#define DEVICE_NAME "usb1"

void print_arr(u8* d, u16 len, u8* name){
    printk("%s[%d] = %*ph\n", name, len, len, d);
}

/* File operation functions */
static int usb1_open(struct inode *inode, struct file *file){
    struct usb_device_data *data = container_of(inode->i_cdev, struct usb_device_data, cdev);
    file->private_data = data;

    /* Do nothing */

    return 0;
}

static int usb1_release(struct inode *inode, struct file *filp)
{
    struct usb_device_data *usb1 = container_of(inode->i_cdev, struct usb_device_data, cdev);
    filp->private_data = usb1;

    /* Do nothing */

    return 0;
}

static ssize_t usb1_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct usb_device_data *data = filp->private_data;
    char *kbuf;
    int ret;
    u16 MaxPktSize;
    u8 epNum_bulk_out_addr, out;

    // schedule_work(&data->re_request_work);
    if (!data)
    {
        pr_err("NULL data\n");
        return -EFAULT;
    }

    if (!data->done_configured){
        return -2;
    }

    MaxPktSize = data->InsDeviceDescriptor.ep_bulk_out_wMaxPacketSize;
 
    // // Limit with max pkt size
    // if (count > MaxPktSize) count = MaxPktSize;

    printk("usb1_write -> count = %d\n", count);

    /* Create kernel buffer */
    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;
 
    if (copy_from_user(kbuf, buf, count)) {
        kfree(kbuf);
        return -EFAULT;
    }

    /* First 4 bits are enpoint addr */
    epNum_bulk_out_addr = data->InsDeviceDescriptor.ep_bulk_out_bEndpointAddress & 0x0F;

    /* bit 7 is r/w */
    out = data->InsDeviceDescriptor.ep_bulk_out_bEndpointAddress & 0x80;
    if ((out & 0x80)) {
        printk("Invalid enpoint addr\n");
        return -1;
    }
    // reset buffer idx
    data->RX_index = 0;

    printk("Sending OutData to Arduino\n");
    mutex_lock(&data->hw_lock);
    ret = USB1_OUT_Phase_Bulk(data, epNum_bulk_out_addr, ARDUINO_DEVICE_ADDR, kbuf, count);
    mutex_unlock(&data->hw_lock);
    if (ret < 0) {
        dev_info(data->dev, "Send error\n");
        return -1;
    }
    return count;
}

static ssize_t usb1_read(struct file *filp, char __user *buf, size_t len, loff_t *f_pos){
    // struct usb_device_data *data = filp->private_data;
    // u16 InDataLen;
    // u8 epNum_bulk_in_addr, in;
    // int ret;

    // if (!data->done_configured){
    //     return -2;
    // }

    // /* First 4 bits are enpoint addr */
    // epNum_bulk_in_addr = data->InsDeviceDescriptor.ep_bulk_in_bEndpointAddress & 0x0F;

    // /* bit 7 is r/w */
    // in = data->InsDeviceDescriptor.ep_bulk_in_bEndpointAddress & 0x80;
    // if (!(in & 0x80)) {
    //     printk("Invalid enpoint addr\n");
    //     return -1;
    // }

    // ret = USB1_IN_Phase_Bulk(data, epNum_bulk_in_addr, ARDUINO_DEVICE_ADDR, data->RX, &InDataLen);
    // if (ret < 0) return ret;

    // print_arr(data->RX, InDataLen, "data->RX");

    // data->RX_index = 0;

    // if (copy_to_user(buf, data->RX, InDataLen))
    //     return -EFAULT;

    // return InDataLen;



    struct usb_device_data *data = filp->private_data;
    unsigned int copied;
    long ret;
 
    if (!data->done_configured)
        return -ENODEV;
 
    /* Sleep until the thread puts something in the kfifo */
    ret = wait_event_interruptible_timeout(data->rx_wait,
              !kfifo_is_empty(&data->rx_fifo) || !data->done_configured,
              msecs_to_jiffies(3000));
    if (ret == 0)  return -EAGAIN;          /* no data for 3 s */
    if (ret < 0)   return ret;              /* signal (Ctrl+C) */
    if (!data->done_configured) return -ENODEV;
 
    /* Copy up to len bytes straight from the ring to user space */
    ret = kfifo_to_user(&data->rx_fifo, buf, len, &copied);
    return ret ? ret : copied;
}
 
static long usb1_ioctl(struct file *file,
                       unsigned int cmd,
                       unsigned long arg)
{
    struct usb_device_data *data = file->private_data;

    if (!data)
        return -ENODEV;

    switch (cmd) {
        case BBB_CDC_CONNECTION:
            struct cdc_status status;
            status.connected  = data->done_configured;
            status.full_speed = data->full_speed;

            if (copy_to_user((void __user *)arg,
                            &status,
                            sizeof(status)))
                return -EFAULT;

            return 0;

        case BBB_CDC_INFO:
            struct cdc_info info;
            info.bcdUSB         = data->InsDeviceDescriptor.bcdUSB;
            info.idVendor       = data->InsDeviceDescriptor.idVendor;
            info.idProduct      = data->InsDeviceDescriptor.idProduct;
            info.bcdDevice      = data->InsDeviceDescriptor.bcdDevice;
            info.iManufacturer  = data->InsDeviceDescriptor.iManufacturer;
            info.iProduct       = data->InsDeviceDescriptor.iProduct;
            info.iSerialNumber  = data->InsDeviceDescriptor.iSerialNumber;

            if (copy_to_user((void __user *)arg,
                            &info,
                            sizeof(info)))
                return -EFAULT;

            return 0;

        default:
            return -ENOTTY;
    }
}

/* Descriptor work */
static void descriptor_work_func(struct work_struct *work){
    struct usb_device_data *data = container_of(work, struct usb_device_data, descriptor_work);
    int ret;

    msleep(1000);
    /* Reset */
    USB1_Reset_Speed(data);

    /* Getting descriptor */
    ret = USB1_GetDesc_Transfer(data);
    if (ret < 0) {
        printk("Fail USB1_GetDesc_Transfer\n");
        return;
    }
    else {
#if (PRINT_DESCRIPTOR)
        USB1_Print_DeviceDescriptor(data);
        USB1_Print_DeviceDescriptor2(data);
        USB1_Print_DeviceDescriptorIf0(data);
#endif
        // Setup FIFO, config once
        USB1_SetupBulkFifos(data, 4, 3);

        /* drop old data; no producer is running yet */
        kfifo_reset(&data->rx_fifo);                       
        data->rx_task = kthread_run(usb1_rx_thread, data, "usb1_rx");

        USB1_PrintCoreConfig(data);

        data->done_configured = true;
    }
}

static void descriptor_clear_work_func(struct work_struct *work){
    struct usb_device_data *data = container_of(work, struct usb_device_data, descriptor_clear_work);
    memset(&data->InsDeviceDescriptor, 0, sizeof(data->InsDeviceDescriptor));
    data->done_configured = false;
}

/* File operation */
static const struct file_operations usb_device_fops = {
    .owner =            THIS_MODULE,
    .open =             usb1_open,
    .release =          usb1_release,
    .write =            usb1_write,
    .read =             usb1_read,
    .unlocked_ioctl =   usb1_ioctl,
};

static int usb1_probe(struct platform_device *pdev)
{
    struct usb_device_data *data;
    int ret;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    platform_set_drvdata(pdev, data);

    data->dev = &pdev->dev;
    data->base_usbss = ioremap(BASE_USBSS, 0x1000);
    data->base_usb1ctl = ioremap(BASE_USB1CTL, 0x200);
    data->base_usb1phy = ioremap(BASE_USB1PHY, 0x100);
    data->base_usb1core = ioremap(BASE_USB1CORE, 0x400);
#if (!INDEX_USED)
    data->base_usb1ep0 = ioremap(USB1EP0_base, 0x10); // 16 bytes
#else
    data->base_usb1ep0 = ioremap(BASE_USB1CORE + 0x10, 0x10); // 16 bytes
#endif
    data->base_con_usb1ctrl1 = ioremap(CONTROL_MODULE, 0x1000);

    /* ===== Clock setup (assuming this part is unchanged) */
    data->clk = devm_clk_get(&pdev->dev, "fck-usb1");
    if (IS_ERR(data->clk)) {
        dev_err(&pdev->dev, "Failed to get clock: %ld\n", PTR_ERR(data->clk));
        return PTR_ERR(data->clk);
    }
    ret = clk_prepare_enable(data->clk);
    if (ret) {
        dev_err(&pdev->dev, "Failed to enable clock: %d\n", ret);
        return ret;
    }
    dev_info(&pdev->dev, "usb1 clock rate: %lu Hz\n", clk_get_rate(data->clk));

    // ========== Request IRQ ==========
    data->irq1 = platform_get_irq(pdev, 0);
    if (data->irq1 < 0) {
        dev_err(&pdev->dev, "Failed Formatted: Unable to get IRQ: %d\n", data->irq1);
        return data->irq1;
    }

    //ret = request_irq(data->irq1, USB1_handler, IRQF_SHARED, "usb1", data);
    ret = devm_request_irq(&pdev->dev, data->irq1, USB1_handler, 0, "usb1", data);
    if (ret < 0) {
        dev_err(&pdev->dev, "Unable to request IRQ %d: %d\n", data->irq1, ret);
        return ret;
    }
    dev_info(&pdev->dev, "IRQ1 num = %d\n", data->irq1);

    // ===== Create character device
    ret = alloc_chrdev_region(&data->dev_num, 0, 1, DRIVER_NAME);
    if (ret < 0) {
        dev_err(&pdev->dev, "Failed to allocate chrdev region: %d\n", ret);
        return ret;
    }

    cdev_init(&data->cdev, &usb_device_fops);
    data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&data->cdev, data->dev_num, 1);
    if (ret < 0) {
        dev_err(&pdev->dev, "Failed to add cdev: %d\n", ret);
        //unregister_chrdev_region(&data->dev_num, 1);
        iounmap(data->base_usbss);
        iounmap(data->base_usb1ctl);
        iounmap(data->base_usb1phy);
        iounmap(data->base_usb1core);
        return ret;
    }

    data->class = class_create(THIS_MODULE, "usb1_class");
    if (IS_ERR(data->class)) {
        dev_err(&pdev->dev, "Failed to create class: %ld\n", PTR_ERR(data->class));
        cdev_del(&data->cdev);
        //unregister_chrdev_region(&data->dev_num, 1);
        iounmap(data->base_usbss);
        iounmap(data->base_usb1ctl);
        iounmap(data->base_usb1phy);
        iounmap(data->base_usb1core);
        return PTR_ERR(data->class);
    }

    data->dev = device_create(data->class, &pdev->dev, data->dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(data->dev)) {
        dev_err(&pdev->dev, "Failed to create device: %ld\n", PTR_ERR(data->dev));
        class_destroy(data->class);
        cdev_del(&data->cdev);
        //unregister_chrdev_region(&data->dev_num, 1);
        iounmap(data->base_usbss);
        iounmap(data->base_usb1ctl);
        iounmap(data->base_usb1phy);
        iounmap(data->base_usb1core);
        return PTR_ERR(data->dev);
    }

    dev_info(&pdev->dev, "Created /dev/%s\n", DEVICE_NAME);

    // ===== USB1 init
    USB1_reset(data);
    PHY1_init(data);
    USB1_init(data);
    data->RX_index = 0;

    // clear stuff
    INIT_WORK(&data->descriptor_work, descriptor_work_func);
    INIT_WORK(&data->descriptor_clear_work, descriptor_clear_work_func);

    mutex_init(&data->hw_lock);
    init_waitqueue_head(&data->rx_wait);
    init_waitqueue_head(&data->irq_wait);
    //init_waitqueue_head(&data->irq_wait_reg);
    INIT_KFIFO(data->rx_fifo);

    return 0;
}

static int usb1_remove(struct platform_device *pdev)
{
    struct usb_device_data *data = platform_get_drvdata(pdev);

    cancel_work_sync(&data->descriptor_work);
    cancel_work_sync(&data->descriptor_clear_work);


    /* Mark the device unavailable first. */
    WRITE_ONCE(data->done_configured, false);

    /* Wake any threads blocked on receive data or USB events. */
    wake_up_interruptible(&data->rx_wait);
    wake_up_interruptible(&data->irq_wait);
    //wake_up_interruptible(&data->irq_wait_reg);

    /* Stop the RX thread and wait for it to exit. */
    if (data->rx_task) {
        kthread_stop(data->rx_task);
        data->rx_task = NULL;
}

    USB1_deinit(data);
    PHY1_deinit(data);

    iounmap(data->base_usbss);
    iounmap(data->base_usb1ctl);
    iounmap(data->base_usb1phy);
    iounmap(data->base_usb1core);
    iounmap(data->base_usb1ep0);
    iounmap(data->base_con_usb1ctrl1);

    if (data->dev)
        device_destroy(data->class, data->dev_num);
    if (data->class)
        class_destroy(data->class);
    cdev_del(&data->cdev);

    return 0;
}

static const struct of_device_id usb_device_of_match[] = {
    { .compatible = "usb1-based" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, usb_device_of_match);

static struct platform_driver usb_device_driver = {
    .probe = usb1_probe,
    .remove = usb1_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = usb_device_of_match,
    },
};

module_platform_driver(usb_device_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Soviet");
MODULE_DESCRIPTION("Custom USB1 Device Driver for BeagleBone Black USB1");

