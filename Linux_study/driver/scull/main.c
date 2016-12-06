/*
 * main.c -- the bare scull char module
 *
 * Tested with 2.0.x (x86 and alpha) and 1.2.13 (x86)
 *********/

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#define __NO_VERSION__ /* don't define kernel_verion in module.h */
#include <linux/module.h>
#include <linux/version.h>

//char kernel_version [] = UTS_RELEASE;

#include <linux/kernel.h> /* printk() */
#include <linux/vmalloc.h> /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /* O_ACCMODE */

#include <asm/system.h>   /* cli(), *_flags */
#include <asm/segment.h>  /* memcpy and such */

#include "scull.h"        /* local definitions */
#include <linux/device.h>
#include <asm/uaccess.h>
extern int addtest(int a ,int b);
extern void addtest2();

static struct class *testdrv_class;

static struct class_device *testdrv_class_dev;


/*
 * I don't use static symbols here, because register_symtab is called
 */

int scull_major =   SCULL_MAJOR;
int scull_nr_devs = SCULL_NR_DEVS;    /* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;

Scull_Dev *scull_devices; /* allocated in init_module */

spinlock_t lock;
rwlock_t rwlock;
struct mutex		mutexlock;


/*
 * Different minors behave differently, so let's use multiple fops
 */


struct file_operations *scull_fop_array[]={
    &scull_fops,      /* type 0 */
//    &scull_priv_fops, /* type 1 */
	    &scull_pipe_fops, /* type 2 */
//	    &scull_sngl_fops, /* type 3 */
//	    &scull_user_fops, /* type 4 */
//	    &scull_wusr_fops  /* type 5 */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
#define SCULL_MAX_TYPE 5

unsigned char test_buf[20];

int scull_trim(Scull_Dev *dev)
{
//	    Scull_Dev *next, *dptr;
//	    int qset = dev->qset;   /* "dev" is not-null */
//	    int i;
//	
//	    if (dev->usage)
//	      return -EBUSY; /* scull_open ignores this error and goes on */
//	
//	    for (dptr = dev; dptr; dptr = next) { /* all the list items */
//	        if (dptr->data) {
//	            for (i = 0; i < qset; i++)
//	                if (dptr->data[i])
//	                    kfree(dptr->data[i]);
//	            kfree(dptr->data);
//	            dptr->data=NULL;
//	        }
//	        next=dptr->next;
//	        if (dptr != dev) kfree(dptr); /* all of them but the first */
//	    }
//	    dev->size = 0;
//	    dev->quantum = scull_quantum;
//	    dev->qset = scull_qset;
//	    dev->next = NULL;
    return 0;
}

/*
 * Open and close
 */

int scull_open (struct inode *inode, struct file *filp)
{
//	    int type = TYPE(inode->i_rdev);
//	    int num = NUM(inode->i_rdev);
//	    Scull_Dev *dev; /* device information */
//	
//	    /* manage peculiar types first */
//	    if (type) {
//	        if (type > SCULL_MAX_TYPE) return -ENODEV;
//	        filp->f_op = scull_fop_array[type];
//	        return filp->f_op->open(inode, filp); /* dispatch to specific open */
//	    }
//	
//	    /* type 0, check the device number */
//	    if (num >= scull_nr_devs) return -ENODEV;
//	    dev = &scull_devices[num];
//	
//	    /* now trim to 0 the length of the device if open was write-only */
//	    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
//	        scull_trim(dev); /* ignore errors */
//	
//	    /* and use filp->private_data to point to the device data */
//	    filp->private_data = dev;
//	
//	    MOD_INC_USE_COUNT;
	
//	printk("%d\n",c);
	int minor = MINOR(inode->i_rdev);
	printk("open minor = %d!!!\n",minor);
	
	if(minor == 4)
	{
		printk("open pipe test\n");
		 filp->f_op = &scull_pipe_fops;			//pipe test
		 return filp->f_op->open(inode, filp);
	}
	
	
    return 0;          /* success */
}

void scull_release (struct inode *inode, struct file *filp)
{
//	    MOD_DEC_USE_COUNT;
	printk("close!!\n");
}
/*
 * Follow the list 
 */
Scull_Dev *scull_follow(Scull_Dev *dev, int n)
{
//	    while (n--) {
//	        if (!dev->next) {
//	            dev->next = kmalloc(sizeof(Scull_Dev), GFP_KERNEL);
//	            memset(dev->next, 0, sizeof(Scull_Dev));
//	        }
//	        dev = dev->next;
//	        continue;
//	    }
    return dev;
}

/*
 * Data management: read and write
 */
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)

{
    Scull_Dev *dev = filp->private_data; /* the first listitem */
	    int quantum = dev->quantum;
	    int qset = dev->qset;
	    int itemsize = quantum * qset; /* how many bytes in the listitem */
	    unsigned long f_pos = (unsigned long)(filp->f_pos);
	    int item, s_pos, q_pos, rest;
	
//	    if (f_pos > dev->size)
//	        return 0;
//	    if (f_pos + count > dev->size)
//	        count = dev->size - f_pos;
//	    /* find listitem, qset index and offset in the quantum */
//	    item = f_pos / itemsize;
//	    rest = f_pos % itemsize;
//	    s_pos = rest / quantum; q_pos = rest % quantum;
//	
//	    /* follow the list up to the right position (defined elsewhere) */
//	    dev = scull_follow(dev, item);
//	
//	    if (!dev->data)
//	        return 0; /* don't fill holes */
//	    if (!dev->data[s_pos])
//	        return 0;
//	    if (count > quantum - q_pos)
//	        count = quantum - q_pos; /* read only up to the end of this quantum */
//	
//	    dev->usage++; /* the following call may sleep */
//	    memcpy_tofs(buf, dev->data[s_pos]+q_pos, count);
//	    dev->usage--;
//	
//	    filp->f_pos += count;
//	    return count;
	printk("reading\n");
	//copy_to_user(buf,"bbcc",count);
	copy_to_user(buf,test_buf,count);

	return count;
}
ssize_t scull_write (struct file *filp, const char __user * buf, size_t count, loff_t * pos)

{
    Scull_Dev *dev = filp->private_data;
    Scull_Dev *dptr;
    int quantum = dev->quantum;
    int qset = dev->qset;
    int itemsize = quantum * qset;
    unsigned long f_pos = (unsigned long)(filp->f_pos);
    int item, s_pos, q_pos, rest;
//	
//	    /* find listitem, qset index and offset in the quantum */
//	    item = f_pos / itemsize;
//	    rest = f_pos % itemsize;
//	    s_pos = rest / quantum; q_pos = rest % quantum;
//	
//	    /* follow the list up to the right position */
//	    dptr = scull_follow(dev, item);
//	    if (!dptr->data) {
//	        dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
//	        if (!dptr->data)
//	            return -ENOMEM;
//	        memset(dptr->data, 0, qset * sizeof(char *));
//	    }
//	    if (!dptr->data[s_pos]) {
//	        dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
//	        if (!dptr->data[s_pos])
//	            return -ENOMEM;
//	    }
//	    if (count > quantum - q_pos)
//	        count = quantum - q_pos; /* write only up to the end of this quantum */
//	
//	    dev->usage++; /* the following call may sleep */
//	    memcpy_fromfs(dptr->data[s_pos]+q_pos, buf, count);
//	    dev->usage--;
//	
//	    /* update the size */
//	    if (dev->size < f_pos + count)
//	        dev-> size = f_pos + count;
//	    filp->f_pos += count;
//	    return count;
	printk("writing\n");
	printk("%s\n",buf);
	copy_from_user(test_buf,buf,count);
	return count;
}

/*
 * The ioctl() implementation
 */
long scull_ioctl (struct file *flip, unsigned int cmd, unsigned long arg)
{
	
	unsigned long flags;

	//spinlock & spinlock_irq sample..
	spin_lock_irqsave(&lock,flags);
	//spin_lock(&lock);
	printk("ioctrl %d  %d\n",cmd,arg);
	//spin_unlock(&lock) ; 
	spin_unlock_irqrestore(&lock,flags);


	//rw spinlock
	read_lock(&rwlock);
	printk("read spin lock\n");
	read_unlock(&rwlock);

	write_lock_irqsave(&rwlock,flags);
	printk("write spin lock\n");
	write_unlock_irqrestore(&rwlock, flags);



	//mutex
	mutex_lock(&mutexlock);
	printk("mutex lock\n");
	mutex_unlock(&mutexlock);

	






//	    int err = 0, tmp, size = _IOC_SIZE(cmd); /* the size bitfield in cmd */
//	
//	    /*
//	     * extract the type and number bitfields, and don't decode
//	     * wrong cmds: return EINVAL before verify_area()
//	     */
//	    if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -EINVAL;
//	    if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -EINVAL;
//	
//	    /*
//	     * the direction is a bitmask, and VERIFY_WRITE catches R/W
//	     * transfers. `Type' is user-oriented, while
//	     * verify_area is kernel-oriented, so the concept of "read" and
//	     * "write" is reversed
//	     */
//	    if (_IOC_DIR(cmd) & _IOC_READ)
//	        err = verify_area(VERIFY_WRITE, (void *)arg, size);
//	    else if (_IOC_DIR(cmd) & _IOC_WRITE)
//	        err =  verify_area(VERIFY_READ, (void *)arg, size);
//	    if (err) return err;
//	
//	    switch(cmd) {
//	
//	#ifdef SCULL_DEBUG
//	      case SCULL_IOCHARDRESET:
//	        /*
//	         * reset the counter to 1, to allow unloading in case of problems.
//	         * Use 1, not 0, because the invoking file is still to be closed.
//	         */
//	        mod_use_count_ = 1;
//	        /* don't break: fall through */
//	#endif
//	
//	      case SCULL_IOCRESET:
//	        scull_quantum = SCULL_QUANTUM;
//	        scull_qset = SCULL_QSET;
//	        break;
//	        
//	      case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
//	        scull_quantum = get_user((int *)arg);
//	        break;
//	
//	      case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
//	        scull_quantum = arg;
//	        break;
//	
//	      case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
//	        put_user(scull_quantum, (int *)arg);
//	        break;
//	
//	      case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
//	        return scull_quantum;
//	
//	      case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
//	        tmp = scull_quantum;
//	        scull_quantum = get_user((int *)arg);
//	        put_user(tmp, (int *)arg);
//	        break;
//	
//	      case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
//	        tmp = scull_quantum;
//	        scull_quantum = arg;
//	        return tmp;
//	        
//	      case SCULL_IOCSQSET:
//	        scull_qset = get_user((int *)arg);
//	        break;
//	
//	      case SCULL_IOCTQSET:
//	        scull_qset = arg;
//	        break;
//	
//	      case SCULL_IOCGQSET:
//	        put_user(scull_qset, (int *)arg);
//	        break;
//	
//	      case SCULL_IOCQQSET:
//	        return scull_qset;
//	
//	      case SCULL_IOCXQSET:
//	        tmp = scull_qset;
//	        scull_qset = get_user((int *)arg);
//	        put_user(tmp, (int *)arg);
//	        break;
//	
//	      case SCULL_IOCHQSET:
//	        tmp = scull_qset;
//	        scull_quantum = arg;
//	        return tmp;
//	
//	        /*
//	         * The following two change the buffer size for scullpipe.
//	         * The scullpipe device uses this same ioctl method, just to
//	         * write less code. Actually, it's the same driver, isn't it?
//	         */
//	
//	      case SCULL_P_IOCTSIZE:
//	        scull_p_buffer = arg;
//	        break;
//	
//	      case SCULL_P_IOCQSIZE:
//	        return scull_p_buffer;
//	
//	
//	      default:  /* redundant, as cmd was checked against MAXNR */
//	        return -EINVAL;
//	    }


    return 0;

}

/*
 * The "extended" operations -- only seek
 */

int scull_lseek (struct inode *inode, struct file *filp,
                 off_t off, int whence)
{
//	    Scull_Dev *dev = filp->private_data;
//	    long newpos;
//	
//	    switch(whence) {
//	      case 0: /* SEEK_SET */
//	        newpos = off;
//	        break;
//	
//	      case 1: /* SEEK_CUR */
//	        newpos = filp->f_pos + off;
//	        break;
//	
//	      case 2: /* SEEK_END */
//	        newpos = dev->size + off;
//	        break;
//	
//	      default: /* can't happen */
//	        return -EINVAL;
//	    }
//	    if (newpos<0) return -EINVAL;
//	    filp->f_pos = newpos;
//	    return newpos;
	return 0;
}


/*
 * The different file operations
 */



struct file_operations scull_fops = {
	.owner =	THIS_MODULE,
	.read =scull_read,
	.write = scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};
/*
 * Finally, the module stuff
 */

int __init scull_init(void)
{
    int result, i;

    /*
     * Register your major, and accept a dynamic number
     */
  // int c = addtest(3,5);
//	printk("%d\n",c);
//	addtest2();

    result = register_chrdev(scull_major, "scull", &scull_fops);
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n",scull_major);
        return result;
    }
    if (scull_major == 0) scull_major = result; /* dynamic */

    /* 
     * allocate the devices -- we can't have them static, as the number
     * can be specified at load time
     */
          printk("scull_major = %d\n",scull_major);
    scull_devices = kmalloc(scull_nr_devs * sizeof (Scull_Dev), GFP_KERNEL);
    if (!scull_devices) {
        result = -ENOMEM;
        goto fail_malloc;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof (Scull_Dev));
    for (i=0; i < scull_nr_devs; i++) {
        scull_devices[i].quantum = scull_quantum;
        scull_devices[i].qset = scull_qset;
    }

    /* At this point call the init function for any friend device */
    if ( (result = scull_p_init()) )
        goto fail_pipe;
//	    if ( (result = scull_access_init()) )
//	        goto fail_access;
    /* ... */

#ifndef SCULL_DEBUG
//    register_symtab(NULL); /* otherwise, leave global symbols visible */
#endif

#ifdef SCULL_USE_PROC /* only when available */
    /* this is the last line in init_module */
    proc_register_dynamic(&proc_root, &scull_proc_entry);
#endif

//struct device *device_create(struct class *cls, struct device *parent,dev_t devt, void *drvdata,const char *fmt, ...)__attribute__((format(printf, 5, 6)));


	testdrv_class = class_create(THIS_MODULE, "scull");
	testdrv_class_dev = device_create(testdrv_class, NULL, MKDEV(scull_major, 0), NULL, "scull0");
	testdrv_class_dev = device_create(testdrv_class, NULL, MKDEV(scull_major, 4), NULL, "scull4");			//for pipe test

	spin_lock_init(&lock);
	rwlock_init(&rwlock);
	mutex_init(&mutexlock);


	printk("load ok!\n");
    return 0; /* succeed */

//	  fail_access: scull_p_cleanup();
	  fail_pipe:   kfree(scull_devices);
  fail_malloc: unregister_chrdev(scull_major, "scull");
    return result;
}

void __exit scull_exit(void)
{
	int i;
	device_destroy(testdrv_class, MKDEV(scull_major, 0)); 
		device_destroy(testdrv_class, MKDEV(scull_major, 4)); 
		class_destroy(testdrv_class);
	
	unregister_chrdev(scull_major, "scull");



//	#ifdef SCULL_USE_PROC
//	    proc_unregister(&proc_root, scull_proc_entry.low_ino);
//	#endif
//	
//	    for (i=0; i<scull_nr_devs; i++)
//	        scull_trim(scull_devices+i);
//	    kfree(scull_devices);
//	
//	    /* and call the cleanup functions for friend devices */
	    scull_p_cleanup();
//	    scull_access_cleanup();
	printk("exit ok\n");

}

module_init(scull_init);
module_exit(scull_exit);



MODULE_LICENSE("GPL");



