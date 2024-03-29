#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <asm/ioctl.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>

#include "./../hw_driver.h"							// IOCTL setting header file
#include "./fpga_dot_font.h"					// DOT header file

#define KERNEL_TIMER_MAJOR 242
#define KERNEL_TIMER_MINOR 0
#define KERNEL_TIMER_NAME "dev_driver"
#define IOM_FND_ADDRESS 0x08000004 				// pysical address, FND
#define IOM_LED_ADDRESS 0x08000016 				// pysical address, LED
#define IOM_FPGA_DOT_ADDRESS 0x08000210 		// pysical address, DOT
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090	// pysical address - 32 Byte (16 * 2), TEXT_LCD
#define MAX_BUFF 32								// TEXT_LCD
#define LINE_BUFF 16							// TEXT_LCD

/***************************** FOR TIMER *****************************/

int kernel_timer_open(struct inode *, struct file *);
int kernel_timer_release(struct inode *, struct file *);
int kernel_timer_ioctl(struct file *, unsigned int, unsigned long);
static void kernel_timer_function(unsigned long);

static struct file_operations kernel_timer_fops =
{ .open = kernel_timer_open,
  .unlocked_ioctl = kernel_timer_ioctl,
  .release = kernel_timer_release };

static struct Ioctl_info mydata;				// struct Ioctl_info is in hw_driver.h
static struct timer_list timer;

static int kernel_timer_usage = 0;

/***************************** FOR TIMER *****************************/

/***************************** FOR DEVICE *****************************/

static unsigned char *iom_fpga_fnd_addr;							// FND
static unsigned char fnd_init[4] = {0};								// FND
static int fnd_count = 0;											// FND

static unsigned char *iom_fpga_led_addr;							// LED
static unsigned char led_init = (unsigned char)0;					// LED
static unsigned char led_number[8] = {128, 64, 32, 16, 8, 4, 2, 1};	// LED

static unsigned char *iom_fpga_dot_addr;							// DOT

static unsigned char *iom_fpga_text_lcd_addr;						// TEXT_LCD
static unsigned char line_1[LINE_BUFF];								// TEXT_LCD
static unsigned char line_2[LINE_BUFF];								// TEXT_LCD

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

// check the current index location of FND number
// return : current index number
int check_index(unsigned char *gdata){
	unsigned char value[4];
	int i;
	int index = -1;

	memcpy(&value, gdata, sizeof(value));
	
	for (i=0; i<4; i++){
		if(value[i] != 0){
			index = i;
		}
	}

	return index;
}
/***************************** FND FUNCTION *****************************/

/***************************** LED FUNCTION *****************************/
// when write to led device  ,call this function
// flag : 0 (end phase), 1 (execution phase)
ssize_t iom_led_write(unsigned char *gdata, int flag) 
{
	unsigned char value;
	unsigned short _s_value;
	const char *tmp = gdata;

	memcpy(&value, tmp, sizeof(value));
	if ( flag != 0 ) {
		value = led_number[(int)value-1];
	}

    _s_value = (unsigned short)value;
    outw(_s_value, (unsigned int)iom_fpga_led_addr);

	return sizeof(value);
}

/***************************** LED FUNCTION *****************************/

/***************************** DOT FUNCTION *****************************/
// when write to fpga_dot device  ,call this function
ssize_t iom_fpga_dot_write(unsigned char *gdata) 
{
	int i;

	unsigned char value[10];
	unsigned short int _s_value;
	const char *tmp = gdata;

	memcpy(&value, tmp, sizeof(value));

	for(i=0;i<sizeof(value);i++)
    {
        _s_value = value[i] & 0x7F;
		outw(_s_value,(unsigned int)iom_fpga_dot_addr+i*2);
    }
	
	return sizeof(value);
}
/***************************** DOT FUNCTION *****************************/

/***************************** TEXT_LCD FUNCTION *****************************/
// when write to fpga_text_lcd device  ,call this function
ssize_t iom_fpga_text_lcd_write(char *gdata) 
{
	int i;

    unsigned short int _s_value = 0;
	unsigned char value[33];
	const char *tmp = gdata;

	memcpy(&value, tmp, MAX_BUFF);
	value[MAX_BUFF] = 0;

	for(i=0; i < MAX_BUFF; i++)
    {
        _s_value = (((value[i] & 0xFF) << 8) | (value[i + 1] & 0xFF));
		outw(_s_value,(unsigned int)iom_fpga_text_lcd_addr+i);
        i++;
    }

	return MAX_BUFF;
}

// for shifting lcd_text string
void shift_text(unsigned char *gdata){
	unsigned char value[LINE_BUFF];
	unsigned char tmp_value[LINE_BUFF];
	const char *tmp = gdata;
	int i;

	memcpy(&value, tmp, LINE_BUFF);
	memcpy(&tmp_value, tmp, LINE_BUFF);

	for (i=0; i<LINE_BUFF-1; i++){
		value[i+1] = tmp_value[i];
	}
	value[0] = tmp_value[LINE_BUFF-1];
	
	memcpy(gdata, value, LINE_BUFF);
}
/***************************** TEXT_LCD FUNCTION *****************************/

/***************************** TIMER FUNCTION *****************************/
// Executed when timer is expired
static void kernel_timer_function(unsigned long data) {
	struct Ioctl_info *p_data = (struct Ioctl_info*)data;
	int index_value;
	unsigned char specific_data;
	unsigned char value[4];
	unsigned char string_lcd[MAX_BUFF];

	// check current cnt number. 
	p_data->cnt--;
	// cnt is over
	if( (int)p_data->cnt <= 0 ) {
		iom_fpga_fnd_write(fnd_init);
		iom_led_write(&led_init, 0);
		iom_fpga_dot_write(fpga_set_blank);
		memset(string_lcd, ' ', MAX_BUFF);
		iom_fpga_text_lcd_write(string_lcd);

		del_timer(&timer);
		return;
	} else {
		// Move or add fnd number.
		memcpy(&value, p_data->value, sizeof(value));

		index_value = check_index(p_data->value); 
		value[index_value] = value[index_value]+1;
		if (value[index_value] > 8) {
			value[index_value] = 1;
		}
		// After one rotation, Move FND number to the right
		if (fnd_count == 0) {
			// When fnd index is at the far right
			if (index_value == 3){
				value[0] = value[index_value];
				value[index_value] = 0;
				index_value = 0;
			} else {
				value[index_value+1] = value[index_value];
				value[index_value] = 0;
				index_value++;
			}
			fnd_count = 8;
		}

		specific_data = value[index_value];
		memcpy(p_data->value, &value, sizeof(value));

		// shift lcd_text string
		shift_text(line_1);
		shift_text(line_2);
		memcpy(string_lcd, line_1, LINE_BUFF);
		memcpy(string_lcd+LINE_BUFF, line_2, LINE_BUFF);

		// Device control
		iom_fpga_fnd_write(value);
		fnd_count--;
		iom_led_write(&specific_data, 1);
		iom_fpga_dot_write(fpga_number[specific_data]);
		iom_fpga_text_lcd_write(string_lcd);

		// add timer 
		timer.expires = get_jiffies_64() + (mydata.interval/10 * HZ);
		timer.data = (unsigned long)&mydata;
		timer.function = kernel_timer_function;

		add_timer(&timer);
	}
}


/***************************** TIMER FUNCTION *****************************/

/***************************** MODULE IOCTL *****************************/
int kernel_timer_ioctl(struct file * mfile, unsigned int cmd, unsigned long arg){
	printk("The kernel_timer_ioctl() function has been called\n");
	int index;
	unsigned char specific_value;
	char student_num[] = "120220184       ";
	char student_name[] = "JungGyeongHwan  ";
	unsigned char string_lcd[MAX_BUFF];
	
	switch (cmd) {
		// When ioctl is SET_OPTION, init information of timer device driver 
		case SET_OPTION:
			printk("SET_OPTION\n");
			if (copy_from_user(&mydata, (void __user *)arg, sizeof(mydata))) {
				return -EFAULT;
			}
			memcpy(line_1, student_num, LINE_BUFF);
			memcpy(line_2, student_name, LINE_BUFF);

			memcpy(string_lcd, line_1, LINE_BUFF);
			memcpy(string_lcd+LINE_BUFF, line_2, LINE_BUFF);
			del_timer_sync(&timer);

			// DEVICE INIT MODE
			index = check_index(mydata.value);
			specific_value = mydata.value[index];
			fnd_count = 8;

			iom_fpga_fnd_write(mydata.value);
			fnd_count--;
			iom_led_write(&specific_value, 1);
			iom_fpga_dot_write(fpga_number[specific_value]);
			iom_fpga_text_lcd_write(string_lcd);

			break;

		// When ioctl is COMMAND, add TIMER device
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

	init_timer(&timer); 												// TIMER

	// Mapping I/O address space
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);					// FND
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);					// LED
	iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);			// DOT
	iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);	// TEXT_LCD


	printk("init module\n");
	return 0;
}

void __exit kernel_timer_exit(void)
{
	printk("kernel_timer_exit\n");

	del_timer_sync(&timer); 			// TIMER
	// Unmapping I/O address space
	iounmap(iom_fpga_fnd_addr);			// FND
	iounmap(iom_fpga_led_addr);			// LED
	iounmap(iom_fpga_dot_addr);			// DOT
	iounmap(iom_fpga_text_lcd_addr);	// TEXT_LCD

	unregister_chrdev(KERNEL_TIMER_MAJOR, KERNEL_TIMER_NAME);
}

/***************************** MODULE INIT & EXIT *****************************/

module_init( kernel_timer_init);
module_exit( kernel_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");