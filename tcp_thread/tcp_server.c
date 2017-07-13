/*************************************************************************
	> File Name: tcp_server.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Wed 12 Jul 2017 05:05:04 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static void usage(char *proc)
{
	printf("%s[local_ip][local_port]",proc);
}

int startup(const char *_ip,int _port)
{
	//1.建立套接字
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("sock");
		return 2;
	}
	//2.将套接字信息填充
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_port);
	local.sin_addr.s_addr=inet_addr(_ip);
	
	//3.绑定套接字
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		return 3;
	}

	//4.监听
	if(listen(sock,10)<0)
	{
		perror("listen");
		return 4;
	}
	return sock;
}

void* handlerRequest(void* arg)
{
	int new_fd=(int)arg;
	while(1)
	{
		char buf[1024];
		ssize_t s=read(new_fd,buf,sizeof(buf)-1);
		if(s>0)
		{
			buf[s]=0;
			printf("client echo # :%s\n",buf);
		}
		else
		{
			printf("read done..break\n");
			break;
		}
	}
}

int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		return 1;
	}

	//1.启动
	int listen_sock=startup(argv[1],atoi(argv[2]));
	while(1)
	{
		//2.建立接受客户端的套接字,接受套接字
		struct sockaddr_in client;
		socklen_t size=sizeof(client);
		int new_fd=accept(listen_sock,(struct sockaddr*)&client,&size);
		if(new_fd<0)
		{
			perror("accept");
			continue;
		}

		printf("get a new client->%s:%d\n",inet_ntoa(client.sin_addr),\
				ntohs(client.sin_port));
		//3.创建线程
		pthread_t id;
		pthread_create(&id,NULL,handlerRequest,(void*)new_fd);
		pthread_detach(id);
	}
	return 0;
}

