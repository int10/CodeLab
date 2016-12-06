#include "stdio.h"
int main()
{
	unsigned char a[]={3,6,2,4,12,52,9,33,14,27};

	unsigned char i,j,temp;
	unsigned int size = sizeof(a)/sizeof(char);

	for(j=0;j<size;j++)
	{
		for(i=0;i<size-j;i++)
		{
			if(a[i]>a[i+1])
			{
				temp=a[i];
				a[i]=a[i+1];
				a[i+1]=temp;
			}
		}
	}
	for(i = 0;i<10;i++)
		printf("%d  ",a[i]);

	return 0;
}

