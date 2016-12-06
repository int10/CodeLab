/*

* for learn kobject, kset, ktype

*/

#include <linux/kobject.h>

#include <linux/string.h>

#include <linux/sysfs.h>

#include <linux/slab.h>

#include <linux/module.h>

#include <linux/init.h>



struct foo_obj {

	struct kobject kobj;

	int foo;

	int soo;

	int coo;

};

#define to_foo_obj(x)       container_of(x, struct foo_obj, kobj)



struct foo_attribute {

	struct attribute attr;

	ssize_t(*show)(struct foo_obj *foo, struct foo_attribute *attr, char *buf);

	ssize_t(*store)(struct foo_obj *foo, struct foo_attribute *attr,char *buf, size_t count);

};

#define to_foo_attr(x)     container_of(x, struct foo_attribute, attr)



/* 公共的属性操作方法 */

static ssize_t foo_attr_show(struct kobject*kobj, struct attribute *attr,char*buf)
{

	struct foo_attribute *attribute;

	struct foo_obj *foo;

	printk("%s\n",__FUNCTION__);


	attribute= to_foo_attr(attr);

	foo= to_foo_obj(kobj);



	if(!attribute->show)

		return-EIO;



	return attribute->show(foo, attribute, buf);

}



static ssize_t foo_attr_store(struct kobject *kobj, struct attribute *attr,char *buf, size_t len)

{

	struct foo_attribute *attribute;

	struct foo_obj *foo;

	printk("%s\n",__FUNCTION__);

	attribute= to_foo_attr(attr);

	foo= to_foo_obj(kobj);



	if(!attribute->store)

		return-EIO;



	return attribute->store(foo, attribute, buf, len);

}



static const struct sysfs_ops foo_sysfs_ops= {

	.show       = foo_attr_show,

	.store       = foo_attr_store,

};



/* 销毁函数，当引用计数(kref)减为0时调用 */

static void foo_release(struct kobject*kobj)

{

	struct foo_obj *foo;
	printk("%s\n",__FUNCTION__);
	foo= to_foo_obj(kobj);

	kfree(foo);

}



/* 属性操作方法 */

static ssize_t foo_show(struct foo_obj*foo_obj, struct foo_attribute *attr,char*buf)

{
	printk("%s\n",__FUNCTION__);
	return sprintf(buf, "%d\n", foo_obj->foo);

}



static ssize_t foo_store(struct foo_obj*foo_obj, struct foo_attribute *attr,char *buf, size_t count)

{
	printk("%s\n",__FUNCTION__);
	sscanf(buf,"%du", &foo_obj->foo);

	return count;

}



static ssize_t soo_show(struct foo_obj*foo_obj, struct foo_attribute *attr,

						char*buf)

{
	printk("%s\n",__FUNCTION__);
	return sprintf(buf, "%d\n", foo_obj->soo);

}



static ssize_t soo_store(struct foo_obj *foo_obj, struct foo_attribute *attr, char *buf, size_t count)
{
	printk("%s\n",__FUNCTION__);
	sscanf(buf,"%du", &foo_obj->soo);
	return count;

}



/*

*构造文件属性

*/

static struct foo_attribute foo_attribute =

	__ATTR(foo,0666, foo_show, foo_store);



static struct foo_attribute soo_attribute =

	__ATTR(soo,0666, soo_show, soo_store);



static struct foo_attribute coo_attribute =

	__ATTR(coo,0666, soo_show, soo_store);



/*

*添加foo_attribute.attr和soo_attribute.attr为foo_obj的默认属性

*/

static struct attribute*foo_default_attrs[] = {

	&foo_attribute.attr,

	&soo_attribute.attr,

	NULL,

};



/*

*定义foo_obj公共属性和操作

*/

static struct kobj_type foo_ktype = {

	.sysfs_ops        = &foo_sysfs_ops,

	.release   = foo_release,

	.default_attrs= foo_default_attrs,

};



static struct kset *kset_example;

static struct foo_obj *foo_obj;

static struct foo_obj *soo_obj;



static struct foo_obj *create_foo_obj(char *name)

{

	struct foo_obj *foo;

	int ret;

	printk("%s\n",__FUNCTION__);

	foo= kzalloc(sizeof(*foo), GFP_KERNEL);

	if(!foo)

		return NULL;



	foo->kobj.kset= kset_example;

	foo->foo= 6;

	foo->soo= 7;



	ret= kobject_init_and_add(&foo->kobj, &foo_ktype, NULL, "%s",name);

	if(ret) {

		kobject_put(&foo->kobj);

		return NULL;

	}



	/*通知用户空间 */

	kobject_uevent(&foo->kobj,KOBJ_ADD);



	return foo;

}



static void destroy_foo_obj(struct foo_obj*foo)

{
	printk("%s\n",__FUNCTION__);
	kobject_put(&foo->kobj);

}



static int __init example_init(void)

{
	printk("%s\n",__FUNCTION__);
	kset_example= kset_create_and_add("kset_example", NULL, NULL);

	if(!kset_example)

		return-ENOMEM;



	foo_obj= create_foo_obj("foo");

	if(!foo_obj)

		goto foo_error;



	soo_obj= create_foo_obj("soo");

	if(!soo_obj)

		goto soo_error;



	/*添加新的属性 */

	sysfs_create_file(&foo_obj->kobj,&coo_attribute.attr);



	return 0;



soo_error:

	destroy_foo_obj(foo_obj);

foo_error:

	kset_unregister(kset_example);



	return-EINVAL;

}



static void __exit example_exit(void)

{
	printk("%s\n",__FUNCTION__);
	sysfs_remove_file(&foo_obj->kobj,&coo_attribute.attr);

	destroy_foo_obj(soo_obj);

	destroy_foo_obj(foo_obj);

	kset_unregister(kset_example);

}



module_init(example_init);

module_exit(example_exit);



MODULE_LICENSE("GPL");

MODULE_AUTHOR("CJOK<cjok.liao@gmail.com>");
