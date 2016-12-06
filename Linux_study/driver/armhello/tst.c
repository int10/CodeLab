#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 


int addtest(int a,int b)
{
	printk("addtest");
	return (a+b);
}
void addtest2()
{
	printk("addtest2");
	return ;

}

static int __init tst_start(viod)
{
printk("Loading my so simple module...\n");
printk("I love Ubuntu\n");
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
