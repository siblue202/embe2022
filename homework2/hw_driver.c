#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <asm/ioctl.h>
#include <linux/slab.h>

#include <asm/io.h> // FND
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/delay.h>

#include "hw_driver.h"

#define KERNEL_TIMER_MAJOR 242
#define KERNEL_TIMER_MINOR 0
#define KERNEL_TIMER_NAME "dev_driver"
#define IOM_FND_ADDRESS 0x08000004 // pysical address, FND

/***************************** FOR TIMER *****************************/

int kernel_timer_open(struct inode *, struct file *);
int kernel_timer_release(struct inode *, struct file *);
int kernel_timer_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
static void kernel_timer_function(unsigned long);

static struct file_operations kernel_timer_fops =
{ .open = kernel_timer_open,
  .unlocked_ioctl = kernel_timer_ioctl,
  .release = kernel_timer_release };

static struct Ioctl_info mydata;
static struct timer_list timer;

static int kernel_timer_usage = 0;

/***************************** FOR TIMER *****************************/

/***************************** FOR FND *****************************/

static unsigned char *iom_fpga_fnd_addr;

/***************************** FOR FND *****************************/

/***************************** FND FUNCTION *****************************/
// when write to fnd device  ,call this function
ssize_t iom_fpga_fnd_write(unsigned char *gdata) 
{
	// int i;
	unsigned char value[4];
	unsigned short int value_short = 0;
	// const char *tmp = gdata;

	// if (copy_from_user(&value, tmp, 4))
	// 	return -EFAULT;
	// value = gdata;
	memcpy(value, *gdata, 4*sizeof(char));

    value_short = value[0] << 12 | value[1] << 8 |value[2] << 4 |value[3];
    outw(value_short,(unsigned int)iom_fpga_fnd_addr);	    

	return sizeof(value);
}

int check_index(unsigned char *gdata){
	unsigned char value[4];
	int i;
	int index = -1;
	// value = gdata;
	memcpy(value, *gdata, 4*sizeof(char));
	
	for (i=0; i<4; i++){
		if(value[i] != 0){
			index = i;
		}
	}

	return index;
}
/***************************** FND FUNCTION *****************************/


/***************************** TIMER FUNCTION *****************************/

static void kernel_timer_function(unsigned long data) {
	struct Ioctl_info *p_data = (struct Ioctl_info*)data;
	int index_init;
	unsigned char value[4];

	// count check
	p_data->cnt--;
	if( p_data->cnt <0 ) {
		return;
	}

	// data's init data change
	// value = p_data->init;
	memcpy(value, *(p_data->init), 4*sizeof(char));
	index_init = check_index(p_data->init); 
	value[index_init] = value[index_init]+1;
	if (value[index_init] > 8) {
		value[index_init] = 1;
	}
	// p_data->init = value;
	memcpy(p_data->init, value, 4*sizeof(char));
	
	// device control
	iom_fpga_fnd_write(value);

	// add timer 
	timer.expires = get_jiffies_64() + (mydata.interval/10 * HZ);
	timer.data = (unsigned long)&mydata;
	timer.function = kernel_timer_function;

	add_timer(&timer);
}


/***************************** TIMER FUNCTION *****************************/

/***************************** MODULE IOCTL *****************************/


int kernel_timer_ioctl(struct inode * minode, struct file * mfile, unsigned int cmd, unsigned long arg){
	printk("The kernel_timer_ioctl() function has been called\n");
	
	switch (cmd) {
		case SET_OPTION:
			printk("SET_OPTION\n");
			if (copy_from_user(&mydata, (void __user *)arg, sizeof(mydata))) {
				return -EFAULT;
			}
			printk("[TIMER_INTERVAL] : %lu\n", mydata.interval);
    		printk("[TIMER_CNT] : %lu\n", mydata.cnt);
    		printk("[TIMER_INIT] : %u\n", mydata.init);

			del_timer_sync(&timer);

			// FND INIT MODE
			iom_fpga_fnd_write(mydata.init);

			break;

		case COMMAND:
			printk("COMMAND\n");

			timer.expires = get_jiffies_64() + (mydata.interval/10 * HZ);
			timer.data = (unsigned long)&mydata;
			timer.function = kernel_timer_function;

			add_timer(&timer);

			break;
		default:
			printk(KERN_WARNING "unsupported command %d\n", cmd);
			return -EFAULT;
	}
	return 0;
}


/***************************** MODULE IOCTL *****************************/

/***************************** MODULE OPEN & RELEASE *****************************/
int kernel_timer_release(struct inode *minode, struct file *mfile) {
	printk("kernel_timer_release\n");
	kernel_timer_usage = 0;
	return 0;
}

int kernel_timer_open(struct inode *minode, struct file *mfile) {
	printk("kernel_timer_open\n");
	if (kernel_timer_usage != 0) {
		return -EBUSY;
	}
	kernel_timer_usage = 1;
	return 0;
}
/***************************** MODULE OPEN & RELEASE *****************************/

/***************************** MODULE INIT & EXIT *****************************/

int __init kernel_timer_init(void)
{
	int result;


	printk("kernel_timer_init\n");


	result = register_chrdev(KERNEL_TIMER_MAJOR, KERNEL_TIMER_NAME, &kernel_timer_fops);
	if(result <0) {
		printk( "error %d\n",result);
		return result;
	}
    printk( "dev_file : /dev/%s , major : %d\n",KERNEL_TIMER_NAME,KERNEL_TIMER_MAJOR);

	init_timer(&timer); // TIMER
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4); // FND

	printk("init module\n");
	return 0;
}

void __exit kernel_timer_exit(void)
{
	printk("kernel_timer_exit\n");

	del_timer_sync(&timer); // TIMER
	iounmap(iom_fpga_fnd_addr); // FND

	unregister_chrdev(KERNEL_TIMER_MAJOR, KERNEL_TIMER_NAME);
}

/***************************** MODULE INIT & EXIT *****************************/

module_init( kernel_timer_init);
module_exit( kernel_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");