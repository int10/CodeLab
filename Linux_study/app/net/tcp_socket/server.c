#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
int main()
{
int sockfd,new_fd;
struct sockaddr_in my_addr;
struct sockaddr_in their_addr;
int sin_size;
//����TCP�׽ӿ�
if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
{
printf("create socket error");
perror("socket");
exit(1);
}
//��ʼ���ṹ�壬����2323�˿�
my_addr.sin_family = AF_INET;
my_addr.sin_port = htons(2323);
my_addr.sin_addr.s_addr = INADDR_ANY;
bzero(&(my_addr.sin_zero),8);
//���׽ӿ�
if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
{
perror("bind socket error");
exit(1);
}
//���������׽ӿ�
if(listen(sockfd,10)==-1)
{
perror("listen");
exit(1);
}

//�ȴ�����
//	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);

		printf("server is run.\n");
		//����������ӣ�������һ��ȫ�µ��׽���
		if((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size))==-1)
		{
			perror("accept");
			exit(1);
		}
		printf("accept success.\n");
		//����һ���ӽ�������ɺͿͻ��˵ĻỰ�������̼�������
		if(!fork())
		{
			printf("create new thred success.\n");
			//��ȡ�ͻ��˷�������Ϣ
			int numbytes;
			char buff[256];
			memset(buff,0,256);
			if((numbytes = recv(new_fd,buff,sizeof(buff),0))==-1)
			{
				perror("recv");
				exit(1);
			}
			printf("%s",buff);
			//���ӿͻ��˽��յ�����Ϣ�ٷ��ؿͻ���
			if(send(new_fd,buff,strlen(buff),0)==-1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		printf("One connect end\n");
		close(new_fd);
	}
	printf("exit!\n");
	close(sockfd);
}