/*

*       forlearn kobject

*/

#include <linux/module.h>

#include <linux/init.h>

#include <linux/kobject.h>

#include <linux/sysfs.h>

#include <linux/string.h>

 

static int n = 6;

/* 当用户空间读取属性文件时，有调用到此方法把指定的值编码后放入缓冲区 */

static ssize_t sc_show(struct kobject*kobj, 

                            struct kobj_attribute *attr, char *buf)

{

         return sprintf(buf, "%d\n", n);

}

/* 当用户空间写入属性文件时，有调用到此方法把指定的值解码后放入传递给相应变量 */

static ssize_t sc_store(struct kobject*kobj, 

                            struct kobj_attribute *attr, const char *buf, size_t count)

{

         sscanf(buf,"%du", &n);

         return count;

}

 

/* 构建kobject的属性 */

static struct kobj_attribute sc_attrb = 

         __ATTR(sc_example,0666, sc_show, sc_store);

 

static struct kobject *kobj;

 

static int __init example_init(void)

{

         int ret;

 

         /*create kobject, a dir(kobj_example) in sys/ */

         kobj= kobject_create_and_add("kobj_example", NULL);

         if(!kobj)

                   return- ENOMEM;

         

         /*create a attribute file(sc_example) in kobj_example */

         ret= sysfs_create_file(kobj, &sc_attrb.attr);

         if(ret)

                   goto attr_file_failed;

         return 0;

         

attr_file_failed:

         kobject_put(kobj);

         return ret;

}

 

static void __exit example_exit(void)

{

         sysfs_remove_file(kobj,&sc_attrb.attr);

         kobject_put(kobj);

}

 

module_init(example_init);

module_exit(example_exit);

 

MODULE_LICENSE("Dual BSD/GPL");

MODULE_AUTHOR("CJOK<cjok.liao@gmail.com>");

