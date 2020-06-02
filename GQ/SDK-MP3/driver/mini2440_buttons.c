#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME     "buttons"

//#define DEBUG 
struct button_irq_desc {
    int irq;
    int pin;
    int pin_setting;
    int number;
    char *name;	
};

#if !defined (CONFIG_QQ2440_BUTTONS)
static struct button_irq_desc button_irqs [] = {
    {IRQ_EINT8 , S3C2410_GPG0 ,  S3C2410_GPG0_EINT8  , 0, "KEY0"},
    {IRQ_EINT11, S3C2410_GPG3 ,  S3C2410_GPG3_EINT11 , 1, "KEY1"},
    {IRQ_EINT13, S3C2410_GPG5 ,  S3C2410_GPG5_EINT13 , 2, "KEY2"},
    {IRQ_EINT14, S3C2410_GPG6 ,  S3C2410_GPG6_EINT14 , 3, "KEY3"},
    {IRQ_EINT15, S3C2410_GPG7 ,  S3C2410_GPG7_EINT15 , 4, "KEY4"},
    {IRQ_EINT19, S3C2410_GPG11,  S3C2410_GPG11_EINT19, 5, "KEY5"},
};
#else /* means QQ */
static struct button_irq_desc button_irqs [] = {
    {IRQ_EINT19, S3C2410_GPG11, S3C2410_GPG11_EINT19, 0, "KEY0"},
    {IRQ_EINT11, S3C2410_GPG3,  S3C2410_GPG3_EINT11,  1, "KEY1"},
    {IRQ_EINT2,  S3C2410_GPF2,  S3C2410_GPF2_EINT2,   2, "KEY2"},
    {IRQ_EINT0,  S3C2410_GPF0,  S3C2410_GPF0_EINT0,   3, "KEY3"},
    {       -1,            -1,                 -1,    4, "KEY4"},
    {       -1,            -1,                 -1,    5, "KEY5"},
};
#endif
//static volatile char key_values [] = {'0', '0', '0', '0', '0', '0'};
static int key_values = 0;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;


static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
    struct button_irq_desc *button_irqs = (struct button_irq_desc *)dev_id;
    int down;
    // udelay(0);
    
    /*上升沿触发，GPIO DAT 应该为非0 的数*/
    down = !s3c2410_gpio_getpin(button_irqs->pin);
    if (!down) { 
	//printk("rising\n");
	key_values = button_irqs->number;
        ev_press = 1;
        wake_up_interruptible(&button_waitq);
    }
   else {
	//printk("falling\n");
	ev_press = 0;
	return 0;
   }
    return IRQ_RETVAL(IRQ_HANDLED);
}


static int s3c24xx_buttons_open(struct inode *inode, struct file *file)
{
    int i;
    int err = 0;
    
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
	if (button_irqs[i].irq < 0) {
		continue;
	}

 	/* 设置中断触发方式 IRQ_TYPE_EDGE_FALLING,IRQ_TYPE_EDGE_RISING,IRQ_TYPE_EDGE_BOTH ；我们这里设置为上升沿触发*/
        //err = request_irq(button_irqs[i].irq, buttons_interrupt, IRQ_TYPE_EDGE_BOTH, 
        //                  button_irqs[i].name, (void *)&button_irqs[i]);
        err = request_irq(button_irqs[i].irq, buttons_interrupt, IRQ_TYPE_EDGE_RISING, 
                          button_irqs[i].name, (void *)&button_irqs[i]);
        if (err)
            break;
    }

    if (err) {
        i--;
        for (; i >= 0; i--) {
	    if (button_irqs[i].irq < 0) {
		continue;
	    }
	    disable_irq(button_irqs[i].irq);
            free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
        }
        return -EBUSY;
    }

    ev_press = 0;
    
    return 0;
}


static int s3c24xx_buttons_close(struct inode *inode, struct file *file)
{
    int i;
    
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
	if (button_irqs[i].irq < 0) {
	    continue;
	}
	free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
    }

    return 0;
}


static int s3c24xx_buttons_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long err;
    //int i=0;
    if (!ev_press) {
	if (filp->f_flags & O_NONBLOCK)
	    return -EAGAIN;
	else
	    wait_event_interruptible(button_waitq, ev_press);
    }
    if(count != sizeof key_values)
	return -EINVAL;
    ev_press = 0;
    err = copy_to_user(buff, &key_values, sizeof(key_values));
    return sizeof(key_values);
}

static unsigned int s3c24xx_buttons_poll( struct file *file, struct poll_table_struct *wait)
{
    unsigned int mask = 0;
    poll_wait(file, &button_waitq, wait);
    if (ev_press)
        mask |= POLLIN | POLLRDNORM;
    return mask;
}


static struct file_operations dev_fops = {
    .owner   =   THIS_MODULE,
    .open    =   s3c24xx_buttons_open,
    .release =   s3c24xx_buttons_close, 
    .read    =   s3c24xx_buttons_read,
    .poll    =   s3c24xx_buttons_poll,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

	ret = misc_register(&misc);
#ifdef DEBUG
	printk("debug test\n");//ykz
#endif
	printk (DEVICE_NAME"\tinitialized\n");

	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
