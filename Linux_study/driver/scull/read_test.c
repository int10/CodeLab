#include "stdio.h"
#include <fcntl.h>
int main()
{
	int scull;
	unsigned char buf[20]="aabbcc",buf2[4000];
	scull = open("/dev/scull4",O_RDWR);
	printf("open = %d\n",scull);
	if(scull>0)
	{
		
		read (scull,buf2,10);
		buf2[10] = 0;
		printf("read = %s\n",buf2);
		
	}		
	close(scull);
	return 0;
	
}

