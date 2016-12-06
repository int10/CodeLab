#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define    FIFO_NAME    "myfifo"
#define    BUF_SIZE    1024

int main(void)
{
	int    fd,input_d,size,output_d;
	char    buf[BUF_SIZE] = "Hello procwrite, I come from process named procread!";
	
	FILE *fh;
	if((input_d=open("aa.txt",O_RDWR))<=0)
	{
		perror("open aa.mp4 error !");	
		exit(1);
	}
	if((output_d = open("bb.txt",O_CREAT|O_WRONLY,S_IRWXU))<=0)
	{
		perror("open bb error!");	
		exit(1);
	}	
	while(1)
	{
		size = read(input_d,buf,BUF_SIZE);
		if(size>0)
			write(output_d,buf,BUF_SIZE);
		else
			break;		
	}
	close(input_d);
	close(output_d);
	exit(0);
}

