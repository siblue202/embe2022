#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/slab.h> // jgh

#define KERNEL_TIMER_MAJOR 268 
#define KERNEL_TIMER_MINOR 0
#define KERNEL_TIMER_NAME "kernel_timer"

static int kernel_timer_usage = 0;
static char *buff; // jgh

int kernel_timer_open(struct inode *, struct file *);
int kernel_timer_release(struct inode *, struct file *);
ssize_t kernel_timer_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations kernel_timer_fops =
{ .open = kernel_timer_open, .write = kernel_timer_write,
	.release = kernel_timer_release };

static struct struct_mydata {
	struct timer_list timer;
	int called_num; // jgh
	int count;
};

struct struct_mydata mydata;

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

static void kernel_timer_blink(unsigned long timeout) {
	struct struct_mydata *p_data = (struct struct_mydata*)timeout;

	printk("kernel_timer_blink %d\n", p_data->count);

	p_data->called_num = p_data->called_num + 1; // jgh
	buff[0] = p_data->called_num; // jgh

	p_data->count--; // jgh
	if( p_data->count < 0 ) { // jgh
		return;
	}

	mydata.timer.expires = get_jiffies_64() + (3 * HZ); // jgh
	mydata.timer.data = (unsigned long)&mydata;
	mydata.timer.function = kernel_timer_blink;

	add_timer(&mydata.timer);
}

ssize_t kernel_timer_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) {
	const char *tmp = gdata;
	char kernel_timer_buff = 0;

	printk("write\n");
	// 1 byte
	if (copy_from_user(&kernel_timer_buff, tmp, 1)) {
		return -EFAULT;
	}

	mydata.count = kernel_timer_buff;
	mydata.called_num = 0; // jgh

	printk("data  : %d \n",mydata.count);

	del_timer_sync(&mydata.timer);

	mydata.timer.expires = jiffies + (1 * HZ);
	mydata.timer.data = (unsigned long)&mydata;
	mydata.timer.function	= kernel_timer_blink;

	add_timer(&mydata.timer);
	return 1;
}

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

	init_timer(&(mydata.timer));

	buff = kmalloc(1 * PAGE_SIZE, GFP_KERNEL); // jgh

	printk("init module\n");
	return 0;
}

void __exit kernel_timer_exit(void)
{
	printk("kernel_timer_exit\n");
	kernel_timer_usage = 0;

	// jgh start
	printk("%d\n",  buff[0]);
	kfree(buff);
	// jgh end

	del_timer_sync(&mydata.timer);

	unregister_chrdev(KERNEL_TIMER_MAJOR, KERNEL_TIMER_NAME);
}

module_init( kernel_timer_init);
module_exit( kernel_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");
