/*************************************************************************
	> File Name: tcp_client.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Wed 12 Jul 2017 04:19:00 PM CST
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

static void usage(char* proc)
{
	printf("%s[local_ip][local_port]\n",proc);
}

int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		return 1;
	}

	//1.建立客户端套接字
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("sock");
		return 2;
	}

	//2.创建接受连接的服务器套接字
	struct sockaddr_in remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(atoi(argv[2]));
	remote.sin_addr.s_addr=inet_addr(argv[1]);

	//3.进行连接
	if(connect(sock,(struct sockaddr*)&remote,sizeof(remote))<0)
	{
		perror("connect");
		return 3;
	}

	//4.客户端进行操作
	while(1)
	{
		char buf[1024];
		printf("please enter # ");
		fflush(stdout);
		
		//5.客户端发送数据给服务器
		ssize_t s=read(0,buf,sizeof(buf)-1);
		if(s>0)
		{
			buf[s-1]=0;
			printf("server echo #%s\n",buf);
			write(sock,buf,strlen(buf));
		}
	}
	return 0;
}
