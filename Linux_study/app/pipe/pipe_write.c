#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define    FIFO_NAME    "myfifo.avi"
#define    BUF_SIZE    1024

int main(void)
{
	int    fd;
	char    buf[BUF_SIZE] = "Hello procwrite, I come from process named procread!";
	FILE * input_fd;
	int size;
	int count = 0;

	umask(0);

	if (mkfifo(FIFO_NAME, S_IFIFO | 0666) == -1) {
		//perror("mkfifo error!");
		//exit(1);
	}
	printf("start to open!\n");
	if ((fd = open(FIFO_NAME, O_WRONLY)) == -1) {
		perror("open error!");
		exit(1);
	}
	input_fd = fopen("output.avi","rb");

	while(1)
	{
		size = fread(buf,1,BUF_SIZE,input_fd);
		printf("size = %d   %d\n",size,count++);

		if((size)<=0)
			break;
		//printf("size = %s\n",buf);

		printf("start to write!\n");

		write(fd, buf, size);    /*strlen(buf)+1 是把'\0'也写过去*/
		printf("write end!\n");
	}

	close(fd);
	exit(0);
}