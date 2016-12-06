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


/*********����struct i2c_rdwr_ioctl_data��struct i2c_msg��Ҫ���ں�һ��*******////int10int10 include i2c.h i2c-dev.h can do it..

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
//		/* nmsgs��������������ж��ٿ�ʼ�źţ����ڡ�����ʼʱ�򡱣�ȡ1*/
//	};

/***********������***********/
int main()
{
	int fd,ret;
	struct i2c_rdwr_ioctl_data e2prom_data;
	fd=open("/dev/i2c-0",O_RDWR);
	/*
	dev/i2c-0����ע��i2c-dev.c������ģ�����һ���ɲ������������������ʹ��i2c-dev.c
	*�ķ�ʽ����û�У�Ҳ����Ҫ����ڵ㡣
	*/
	if(fd<0)
	{
		perror("open error");
	}
	e2prom_data.nmsgs=2;
	/*
	*��Ϊ����ʱ���У�������õ�2����ʼ�źţ��ֽڶ������У������Դ˽�
	*e2prom_data.nmsgs����Ϊ2
	*/
	e2prom_data.msgs=(struct i2c_msg*)malloc(e2prom_data.nmsgs*sizeof(struct i2c_msg));
	if(!e2prom_data.msgs)
	{
		perror("malloc error");
		exit(1);
	}

	ioctl(fd,I2C_TIMEOUT,1);/*��ʱʱ��*/
	ioctl(fd,I2C_RETRIES,2);/*�ظ�����*/
	/***write data to e2prom**/

	e2prom_data.nmsgs=1;
	e2prom_data.msgs[0].len=2; //1�� e2prom д��Ŀ��ĵ�ַ��1������
	e2prom_data.msgs[0].addr=0x50;//e2prom �豸��ַ
	e2prom_data.msgs[0].flags=0; //write
	e2prom_data.msgs[0].buf=(unsigned char*)malloc(2);
	e2prom_data.msgs[0].buf[0]=0x10;// e2prom д��Ŀ��ĵ�ַ
	e2prom_data.msgs[0].buf[1]=0x58;//the data to write

	ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret<0)
	{
		perror("ioctl error1");
	}
	sleep(1);
	/******read data from e2prom*******/
	e2prom_data.nmsgs=2;
	e2prom_data.msgs[0].len=1; //e2prom Ŀ�����ݵĵ�ַ
	e2prom_data.msgs[0].addr=0x50; // e2prom �豸��ַ
	e2prom_data.msgs[0].flags=0;//write
	e2prom_data.msgs[0].buf[0]=0x10;//e2prom���ݵ�ַ
	e2prom_data.msgs[1].len=1;//����������
	e2prom_data.msgs[1].addr=0x50;// e2prom �豸��ַ
	e2prom_data.msgs[1].flags=1;//read
	e2prom_data.msgs[1].buf=(unsigned char*)malloc(1);//��ŷ���ֵ�ĵ�ַ��
	e2prom_data.msgs[1].buf[0]=0;//��ʼ��������

	ret=ioctl(fd,I2C_RDWR,(unsigned long)&e2prom_data);
	if(ret<0)
	{
		perror("ioctl error2");
	}
	printf("buff[0]=%x/n",(e2prom_data.msgs[1]).buf[0]);
	/***��ӡ������ֵ��û��Ļ�����Ӧ����ǰ��д��0x58��***/
	close(fd);
	return 0;
}
