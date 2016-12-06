#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/moduleparam.h>


static char *who= "world";
static int times = 1;

module_param(times,int,S_IRUSR);
module_param(who,charp,S_IRUSR);

int addtest(int a,int b)
{
	printk("addtest");
	return (a+b);
}
void addtest2(void)
{
	printk("addtest2");
	return ;

}

static int __init tst_start(void)
{
	int i;
	printk("Loading my so simple module...\n");
	printk("I love Ubuntu\n");
	
	for(i=0;i<times;i++)
		printk(KERN_ALERT "(%d) hello, %s!\n",i,who);

	return 0;
}

static void __exit tst_end(void)
{
printk("From tst module: Goodbye, Tony.\n");
}

module_init(tst_start);
module_exit(tst_end);
EXPORT_SYMBOL(addtest);
EXPORT_SYMBOL(addtest2);
MODULE_LICENSE("GPL");
