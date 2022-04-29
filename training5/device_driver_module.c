#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>

#define DEVICE_DRIVER_NAME "driver_test"
// long number = 0;
int number = 0;
int major_number;
static char msg[4];

int test_device_driver_open(struct inode *, struct file *);
int test_device_driver_release(struct inode *, struct file *);
ssize_t test_device_driver_write(struct file *, const char *, size_t, loff_t *);
ssize_t test_device_driver_read(struct file *filep, char *buffer, size_t length, loff_t *offset);

static struct file_operations device_driver_fops =
{ .open = test_device_driver_open,
  .read = test_device_driver_read,
  .write = test_device_driver_write,
  .release = test_device_driver_release };


int test_device_driver_open(struct inode *minode, struct file *mfile) {
	printk("test_device_driver_open\n");
	return 0;
}

int test_device_driver_release(struct inode *minode, struct file *mfile) {
	printk("test_device_driver_release\n");
	return 0;
}


ssize_t test_device_driver_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) {
	const char *tmp = gdata;
	// char *kernel_buff = msg;


	printk("Write\n");
	if (copy_from_user(&msg, tmp, 4)) {
		return -EFAULT;
	}
	number = simple_strtol(msg, NULL, 10);
	printk("Copy number to kernel buffer : %d \n",number);

	return 1;
}

ssize_t test_device_driver_read(struct file *filep, char *buffer, size_t length, loff_t *offset) {
	/* Number of bytes actually written to the buffer */
	int result = 0;
	int i, tmp, flag;
	unsigned char buf;

	tmp = simple_strtol(msg, NULL, 10);
	printk("kstrtoint result : %d\n", tmp);
	for (i=0; i<4; i++) {
		result += tmp%10;
		tmp /= 10;
	}

	buf = result;
	printk("read() buf : %d \n", buf);

	if (copy_to_user(&buf, buffer, 1)) {
		return -EFAULT;
	}

	return 1;
};

int __init device_driver_init(void)
{

	printk("device_driver_init\n");
	
	major_number = register_chrdev(0, DEVICE_DRIVER_NAME, &device_driver_fops);
	if(major_number <0) {
		printk( "error %d\n",major_number);
		return major_number;
	}

    printk( "dev_file: /dev/%s , major: %d\n", DEVICE_DRIVER_NAME, major_number);
	
	printk("init module\n");
	return 0;
}

void __exit device_driver_exit(void)
{
	printk("device_driver_exit\n");

	unregister_chrdev(major_number, DEVICE_DRIVER_NAME);
}

module_init(device_driver_init);
module_exit(device_driver_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");
