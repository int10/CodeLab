#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#define    FIFO_NAME    "myfifo.yuv"
#define    BUF_SIZE    1024
int main(void)
{
     int     fd;
     char    buf[BUF_SIZE];
	 FILE *output_fd;
	 int size,count = 0;
    umask(0);
	printf("start to open!\n");
     fd = open(FIFO_NAME, O_RDONLY);
	 
	 output_fd = fopen("bb.txt","wb");
	 
	 while(1)
	 {
		printf("start to read!\n");
		size = read(fd, buf, BUF_SIZE);
		printf("read size = %d  %d!\n",size,count++);
		if(size<=0)
			break;
		fwrite(buf,size,1,output_fd);

    // printf("Read content: %s\n", buf);
	 }
     close(fd);
     exit(0);
}