#include "stdio.h"


int main()
{
	int front,end,mid,i;
	int a[] = {1,3,7,19,27,56,78};
	int x = 78;

	front=0;
	end=sizeof(a)/sizeof(int)-1;
	mid=(front+end)/2;
	while(front<end&&a[mid]!=x)
	{
		if(a[mid]<x)front=mid+1;
		if(a[mid]>x)end=mid-1;
		mid=front + (end - front)/2;
	}
	if(a[mid]!=x)
		printf("not found\n");
	else
		printf("found ,it's in %d\n",mid+1);
	return 0;
}

