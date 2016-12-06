#include <time.h>
#include <sys/time.h>
#include <stdio.h>

int main()
{
	time_t tt;
	struct timeval tv,prtv;
	int i = 0;
	long div;
	tt = time(NULL);
	gettimeofday(&tv,NULL);
	printf("time_t sec = %ld\n",tt);
	printf("gettimeofday tv_sec=%ld usec is %ld \n",tv.tv_sec,tv.tv_usec);
	//while(1)
	tv.tv_usec = 0;
	tv.tv_sec = 0;
	prtv.tv_usec = 0;
	prtv.tv_sec = 0;
	for(i = 0;i<10;)
	{
		gettimeofday(&tv,NULL);
		
		
		if(tv.tv_usec>=prtv.tv_usec)
			div = tv.tv_usec-prtv.tv_usec;
		else
			div = 1000000L-(prtv.tv_usec-tv.tv_usec);
		
		//printf("%ld %ld %ld\n",tv.tv_usec,prtv.tv_usec,(long)div);
		
		if(div >= 500000L)
		{
			prtv.tv_usec = tv.tv_usec;
			prtv.tv_sec = tv.tv_sec;
			i++;
			printf("test\n");
		}
		
		
	}
	return 0;
}

