#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/version.h>
#include <linux/workqueue.h>

#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <asm/io.h>
#include <asm/gpio.h>

#include <mach/gpio.h>

#include "./../stopwatch_ioctl.h"					// IOCTL setting header file

#define KERNEL_STOPWATCH_MAJOR 242
#define KERNEL_STOPWATCH_MINOR 0
#define KERNEL_STOPWATCH_NAME "stopwatch"
#define IOM_FND_ADDRESS 0x08000004 				// physical address, FND

/***************************** FOR STOPWATCH *****************************/

int kernel_stopwatch_open(struct inode *, struct file *);
int kernel_stopwatch_release(struct inode *, struct file *);
int kernel_stopwatch_ioctl(struct file *, unsigned int, unsigned long);
static void kernel_stopwatch_function(unsigned long);

irqreturn_t home_inter_handler(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t back_inter_handler(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t volup_inter_handler(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t voldown_inter_handler(int irq, void* dev_id, struct pt_regs* reg);

wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);

static struct work_struct my_work;
static void my_wq_function();
static DECLARE_WORK(my_work, my_wq_function);


static struct file_operations kernel_stopwatch_fops =
{ .open = kernel_stopwatch_open,
  .unlocked_ioctl = kernel_stopwatch_ioctl,
  .release = kernel_stopwatch_release };

static struct timer_list timer;
static int run_stopwatch = 0;
static int pushed_stop = 0;
static int kernel_stopwatch_usage = 0;
static int expired_time = 0; 
static unsigned char stopwatch_value[5] = {0};						// STOPWATCH

/***************************** FOR STOPWATCH *****************************/

/***************************** FOR DEVICE *****************************/

static unsigned char *iom_fpga_fnd_addr;							// FND
static unsigned char fnd_value[4] = {0};							// FND
static unsigned char fnd_init[4] = {0};								// FND

/***************************** FOR DEVICE *****************************/

/***************************** FND FUNCTION *****************************/

// when write to fnd device  ,call this function
ssize_t iom_fpga_fnd_write(unsigned char *gdata) 
{
	unsigned char value[4];
	unsigned short int value_short = 0;

	memcpy(&value, gdata, sizeof(value));

    value_short = value[0] << 12 | value[1] << 8 |value[2] << 4 |value[3];
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);	    

	return sizeof(value);
}

/***************************** FND FUNCTION *****************************/

/***************************** FOR HANDLER *****************************/

// when home button is pushed, this handler will be triggered
irqreturn_t home_inter_handler(int irq, void* dev_id, struct pt_regs* reg) {
	if(run_stopwatch == 0) {
		run_stopwatch = 1;
	}
	
	return IRQ_HANDLED;
}

// when back button is pushed, this handler will be triggered
irqreturn_t back_inter_handler(int irq, void* dev_id, struct pt_regs* reg) {        
		if(run_stopwatch == 1){
			run_stopwatch = 0;
		}
		
		return IRQ_HANDLED;
}

// when vol+ button is pushed, this handler will be triggered
irqreturn_t volup_inter_handler(int irq, void* dev_id,struct pt_regs* reg) {        
	    memset(stopwatch_value, 0, sizeof(stopwatch_value));
		memset(fnd_value, 0, sizeof(fnd_value));
		iom_fpga_fnd_write(&fnd_init);

		return IRQ_HANDLED;
}

// when vol- button is pushed, this handler will be triggered
irqreturn_t voldown_inter_handler(int irq, void* dev_id, struct pt_regs* reg) {		
		if(pushed_stop == 0) {
			pushed_stop = 1;
			schedule_work(&my_work);
		} else {
			pushed_stop = 0;
		}

		return IRQ_HANDLED;
}

// BOTTOM HALF HANDELR(WORKQUEUE)
// this handler is workqueue. voldown_inter_handler schedule this handler.
static void my_wq_function() {
	msleep(3000); 				// sleep 3sec

	if(expired_time >= 30){
		// stop application
		del_timer(&timer);
		memset(stopwatch_value, 0, sizeof(stopwatch_value));
		memset(fnd_value, 0, sizeof(fnd_value));
		iom_fpga_fnd_write(fnd_init);
		__wake_up(&wq_write, 1, 1, NULL);
		run_stopwatch = 0;
		pushed_stop = 0;
		expired_time = 0;
	}
}

/***************************** FOR HANDLER *****************************/

/***************************** STOPWATCH FUNCTION *****************************/

// Executed when timer is expired
static void kernel_stopwatch_function(unsigned long data) {
	unsigned char *p_data = (unsigned char *)data;
	unsigned char tmp[5];

	if (pushed_stop == 1){
		expired_time += 1;
	} else {
		expired_time = 0;
	}

	if (run_stopwatch == 1){
		memcpy(&tmp, p_data, sizeof(tmp));

		// time increase
		tmp[4] += 1;
		if (tmp[4] >= 10) {
			tmp[3] += 1;
			tmp[4] = 0;
		}

		if (tmp[3] >= 10) {
			tmp[2] += 1;
			tmp[3] = 0;
		}
		
		if (tmp[2] >= 6) {
			tmp[1] += 1;
			tmp[2] = 0;
		}

		if (tmp[1] >= 10) {
			tmp[0] += 1;
			tmp[1] = 0;
		}

		if (tmp[0] >= 6) {
			tmp[0] = 0;
			// when min exceeds 60 min. stopwatch is done.
			del_timer(&timer);
			memset(stopwatch_value, 0, sizeof(stopwatch_value));
			memset(fnd_value, 0, sizeof(fnd_value));
			memcpy(p_data, &tmp, sizeof(tmp));
			iom_fpga_fnd_write(fnd_init);
			__wake_up(&wq_write, 1, 1, NULL);
			run_stopwatch = 0;
			pushed_stop = 0;
			expired_time = 0;
			return;
		}

		memcpy(p_data, &tmp, sizeof(tmp));
		memcpy(fnd_value, p_data, sizeof(fnd_value));
		iom_fpga_fnd_write(fnd_value);
	} 

	// add timer repeatably
	timer.expires = get_jiffies_64() + (int)(0.1 * HZ);			// interval : 0.1sec
	timer.data = (unsigned long)&stopwatch_value;
	timer.function = kernel_stopwatch_function;

	add_timer(&timer);
}

/***************************** STOPWATCH FUNCTION *****************************/

/***************************** MODULE IOCTL *****************************/

int kernel_stopwatch_ioctl(struct file * mfile, unsigned int cmd, unsigned long arg){
	printk("The kernel_stopwatch_ioctl() function has been called\n");
	
	if (cmd == COMMAND){
		printk("COMMAND\n");

		if(kernel_stopwatch_usage==1){
            // INIT MODE
			iom_fpga_fnd_write(fnd_init);

			// add timer 
			timer.expires = get_jiffies_64() + (int)(0.1 * HZ);			// interval : 0.1sec
			timer.data = (unsigned long)&stopwatch_value;
			timer.function = kernel_stopwatch_function;

			add_timer(&timer);

			printk("sleep on\n");
            interruptible_sleep_on(&wq_write);
        }
	} else {
		printk(KERN_WARNING "unsupported command %d\n", cmd);
		return -EFAULT;
	}

	return 0;
}

/***************************** MODULE IOCTL *****************************/

/***************************** MODULE OPEN & RELEASE *****************************/
int kernel_stopwatch_release(struct inode *minode, struct file *mfile) {
	printk("kernel_stopwatch_release\n");
	kernel_stopwatch_usage = 0;

	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

	return 0;
}

int kernel_stopwatch_open(struct inode *minode, struct file *mfile) {
	int ret;
	int irq;
	
	printk("kernel_stopwatch_open\n");
	if (kernel_stopwatch_usage != 0) {
		return -EBUSY;
	}
	kernel_stopwatch_usage = 1;


	printk(KERN_ALERT "Open Module\n");

	// int1
	gpio_direction_input(IMX_GPIO_NR(1,11));
	irq = gpio_to_irq(IMX_GPIO_NR(1,11));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq, home_inter_handler, IRQF_TRIGGER_FALLING, "home", 0);

	// int2
	gpio_direction_input(IMX_GPIO_NR(1,12));
	irq = gpio_to_irq(IMX_GPIO_NR(1,12));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq, back_inter_handler, IRQF_TRIGGER_FALLING, "back", 0);

	// int3
	gpio_direction_input(IMX_GPIO_NR(2,15));
	irq = gpio_to_irq(IMX_GPIO_NR(2,15));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq, volup_inter_handler, IRQF_TRIGGER_FALLING, "volup", 0);

	// int4
	gpio_direction_input(IMX_GPIO_NR(5,14));
	irq = gpio_to_irq(IMX_GPIO_NR(5,14));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret=request_irq(irq, voldown_inter_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "voldown", 0);

	return 0;
}
/***************************** MODULE OPEN & RELEASE *****************************/

/***************************** MODULE INIT & EXIT *****************************/

int __init kernel_stopwatch_init(void)
{
	int result;

	printk("kernel_stopwatch_init\n");

	result = register_chrdev(KERNEL_STOPWATCH_MAJOR, KERNEL_STOPWATCH_NAME, &kernel_stopwatch_fops);
	if(result <0) {
		printk( "error %d\n",result);
		return result;
	}
    printk( "dev_file : /dev/%s , major : %d\n",KERNEL_STOPWATCH_NAME,KERNEL_STOPWATCH_MAJOR);

	init_timer(&timer); 												// timer

	// Mapping I/O address space
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);					// FND

	printk("init module\n");
	return 0;
}

void __exit kernel_stopwatch_exit(void)
{
	printk("kernel_stopwatch_exit\n");

	del_timer_sync(&timer); 			// timer
	
	// Unmapping I/O address space
	iounmap(iom_fpga_fnd_addr);			// FND

	unregister_chrdev(KERNEL_STOPWATCH_MAJOR, KERNEL_STOPWATCH_NAME);
}

/***************************** MODULE INIT & EXIT *****************************/

module_init( kernel_stopwatch_init);
module_exit( kernel_stopwatch_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");