#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <netdb.h>
#include <sys/types.h>

#include <sys/socket.h>

int main(int argc,char *argv[])
{

int sockfd,numbytes;
char buf[100];

struct sockaddr_in their_addr;
//int i = 0;
//将基本名字和地址转换


//he = gethostbyname(argv[1]);

//建立一个TCP套接口
if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
{
perror("socket");
printf("create socket error.建立一个TCP套接口失败");
exit(1);
}
//初始化结构体，连接到服务器的2323端口
their_addr.sin_family = AF_INET;
their_addr.sin_port = htons(2323);
// their_addr.sin_addr = *((struct in_addr *)he->h_addr);
inet_aton( "127.0.0.1", &their_addr.sin_addr );


bzero(&(their_addr.sin_zero),8);
//和服务器建立连接
if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr))==-1)
{
perror("connect");
exit(1);
}
//向服务器发送数据
if(send(sockfd,"hello!socket.",6,0)==-1)
{
perror("send");
exit(1);
}
//接受从服务器返回的信息
if((numbytes = recv(sockfd,buf,100,0))==-1)
{
perror("recv");
exit(1);
}
buf[numbytes] = 0;
printf("Recive from server:%s",buf);
//关闭socket
close(sockfd);

return 0;
}