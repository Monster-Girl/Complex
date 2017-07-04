#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

static void usage(const char* proc)
{
	printf("%s[server ip][server port]\n",proc);
}

int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		return 1;
	}

	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		printf("socket");
		return 2;
	}

	struct sockaddr_in remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(atoi(argv[2]));
	remote.sin_addr.s_addr=inet_addr(argv[1]);

	if(connect(sock,(struct sockaddr*)&remote,sizeof(remote))<0)
	{
		perror("connect");
		return 3;
	}

	while(1)
	{
		char buf[1024];
		printf("please enter # ");
		fflush(stdout);
		ssize_t s=read(0,buf,sizeof(buf)-1);
		if(s>0)
		{
			buf[s]=0;
			printf("server echo #%s",buf);
			write(sock,buf,s);
		}  else {
			close(sock);
			break;
		}
	}
}
