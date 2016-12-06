#include "stdio.h"
#include <fcntl.h>
int main()
{
	int scull;
	unsigned char buf[20]="aabbcc",buf2[20];
	scull = open("/dev/scull4",O_RDWR);
	printf("open = %d\n",scull);
	if(scull>0)
	{
		write (scull,buf,strlen(buf)+1);
		printf("write end\n");
	}		
	close(scull);
	return 0;
	
}

