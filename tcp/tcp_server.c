/*************************************************************************
	> File Name: tcp_server.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Tue 20 Jun 2017 03:14:11 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>


static void usage(const char *proc)
{
	printf("%s [local_ip,local_port]\n",proc);
}

int startup(const char *_ip,int _port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket");
		exit(2);
	}

	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_port);
	local.sin_addr.s_addr=inet_addr(_ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(3);
	}

	if(listen(sock,5)<0)
	{
		perror("listen");
		exit(4);
	}
	return sock;
}


int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		return 1;
	}
	int listen_sock=startup(argv[1],atoi(argv[2]));
	while(1)
	{
		struct sockaddr_in client;
		socklen_t len=sizeof(client);
		int new_fd=accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_fd<0)
		{
			perror("accept");
			continue;
		}

		printf("get a new client,%s:%d\n",inet_ntoa(client.sin_addr),\
				ntohs(client.sin_port));


		while(1)
		{
			char buf[1024];
	    	ssize_t s=read(new_fd,buf,sizeof(buf)-1);
	 	    if(s>0)
			{
			    buf[s]=0;
				printf("client:%s\n",buf);
				write(new_fd,buf,strlen(buf));
			}
			else
			{
				printf("read done...break\n");
				break;
			}
		}
	}
}
