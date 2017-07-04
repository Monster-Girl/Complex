#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
static void usage(const char *proc)
{
	printf("%s [local_ip][local_port]\n",proc);
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
	int op = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));


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
	printf("get a new client %s:%d\n",inet_ntoa(client.sin_addr),\
				ntohs(client.sin_port));
	
		pid_t id=fork();
		if(id<0)
		{
			perror("fork");
			close(new_fd);
		}
		else if(id==0)//child
		{
			close(listen_sock);
			if(fork()>0)
			{
				exit(0);
			}
			while(1)
			{

				char buf[1024];
				ssize_t s=read(new_fd,buf,sizeof(buf)-1);
				if(s>0)
				{
					buf[s]=0;
					printf("client:%s",buf);
					write(new_fd,buf,strlen(buf));
				}
				else
				{	
					printf("read done....break\n");
					break;
				}
			}
			close(new_fd);		
			exit(0);
		}
		else //father
		{
			close(new_fd);
		}
	}
}
