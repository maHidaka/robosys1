#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define REG(addr) (*((volatile unsigned int*)(addr)))

MODULE_AUTHOR("Masato Hidaka & Ryuichi Ueda");
MODULE_AUTHOR("Ryuichi Ueda");
MODULE_DESCRIPTION("driver for gpio test");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;



static ssize_t led_write(struct file* flip,const char* buf, size_t count, loff_t* pos)
{
	char c;
	if(copy_from_user(&c,buf,sizeof(char)))
		return -EFAULT;

	int val = ((gpio_base[13]) & (1 << 4)) !=0;

	if(val){
		gpio_base[10] = 1 << 25;
	}else{
		gpio_base[7] = 1 << 25;
	}	
	return 1;
}

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;

	char sushi[] = {'s','o','i','y','a'};	

	if(copy_to_user(buf+size, (const char *)sushi, sizeof(sushi))){
		printk(KERN_INFO "sudhi : copy_to_user failed\n");
		return -EFAULT;
	}
	size += sizeof(sushi);
	return size;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void)
{
	int retval;
	
	gpio_base = ioremap_nocache(0xfe200000, 0xA0);
	
	const u32 sw = 4;	
	const u32 led = 25;	
	const u32 sw_index = sw/10;
	const u32 led_index = led/10;

	const u32 sw_shift = (sw%10)*3;
	const u32 led_shift = (led%10)*3;

	const u32 sw_mask = ~(0x7 << sw_shift);
	const u32 led_mask = ~(0x7 << led_shift);

	gpio_base[sw_index] = (gpio_base[sw_index] & sw_mask);
	gpio_base[led_index] = (gpio_base[led_index] & led_mask) | (0x1 << led_shift);

	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}

	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	cdv.owner = THIS_MODULE;
	retval = cdev_add(&cdv, dev, 1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
		return retval;
	}

	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));


			
	return 0;
} 

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);

	
