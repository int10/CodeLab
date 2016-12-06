/*i2c_test.c
 * hongtao_liu <lht@farsight.com.cn>
 */
#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/i2c.h>

#include <linux/i2c-dev.h>

#define I2C_RETRIES 0x0701
#define I2C_TIMEOUT 0x0702
#define I2C_RDWR 0x0707


/*********定义struct i2c_rdwr_ioctl_data和struct i2c_msg，要和内核一致*******////int10int10 include i2c.h i2c-dev.h can do it..

//	
//	struct i2c_msg
//	{
//		unsigned short addr;
//		unsigned short flags;
//		unsigned short len;
//		unsigned char *buf;
//	};
//	
//	struct i2c_rdwr_ioctl_data
//	{
//		struct i2c_msg *msgs;
//		int nmsgs;
//		/* nmsgs这个数量决定了有多少开始信号，对于“单开始时序”，取1*/
//	};

/***********主程序***********/
int main()
{
	int fd,ret;
	struct i2c_rdwr_ioctl_data e2prom_data;
	fd=open("/dev/i2c-0",O_RDWR);
	/*
	dev/i2c-0是在注册i2c-dev.c后产生的，代表一个可操作的适配器。如果不使用i2c-dev.c
	*的方式，就没有，也不需要这个节点。
	*/
	if(fd<0)
	{
		perror("open error");
	}
	e2prom_data.nmsgs=2;
	/*
	*因为操作时序中，最多是用到2个开始信号（字节读操作中），所以此将
	*e2prom_data.nmsgs配置为2
	*/
	e2prom_data.msgs=(struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
	if(!e2prom_data.msgs)
	{
		perror("malloc error");
		exit(1);
	}

	ioctl(fd,I2C_TIMEOUT,1);/*超时时间*/
	ioctl(fd,I2C_RETRIES,2);/*重复次数*/
	/***write data to e2prom**/

	e2prom_data.nmsgs=1;
	e2prom_data.msgs[0].len=2; //1个 e2prom 写入目标的地址和1个数据
	e2prom_data.msgs[0].addr=0x50;//e2prom 设备地址
	e2prom_data.msgs[0].flags=0; //write
	e2prom_data.msgs[0].buf=(unsigned char*)malloc(2);
	e2prom_data.msgs[0].buf[0]=0x10;// e2prom 写入目标的地址
	e2prom_data.msgs[0].buf[1]=0x58;//the data to write

	ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret<0)
	{
		perror("ioctl error1");
	}
	sleep(1);
	/******read data from e2prom*******/
	e2prom_data.nmsgs=2;
	e2prom_data.msgs[0].len=1; //e2prom 目标数据的地址
	e2prom_data.msgs[0].addr=0x50; // e2prom 设备地址
	e2prom_data.msgs[0].flags=0;//write
	e2prom_data.msgs[0].buf[0]=0x10;//e2prom数据地址
	e2prom_data.msgs[1].len=1;//读出的数据
	e2prom_data.msgs[1].addr=0x50;// e2prom 设备地址
	e2prom_data.msgs[1].flags=1;//read
	e2prom_data.msgs[1].buf=(unsigned char*)malloc(1);//存放返回值的地址。
	e2prom_data.msgs[1].buf[0]=0;//初始化读缓冲

	ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret<0)
	{
		perror("ioctl error2");
	}
	printf("buff[0]=%x/n",(e2prom_data.msgs[1]).buf[0]);
	/***打印读出的值，没错的话，就应该是前面写的0x58了***/
	close(fd);
	return 0;
}
