/*************************************************************************
	> File Name: httpd.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Wed 05 Jul 2017 05:04:57 PM CST
 ************************************************************************/

#include"httpd.h"
void print_log(char *msg,int level)
{
#ifdef STDOUT
	const char *const level_msg[]={
		"SUCCESS",
		"NOTICE",
		"WARNING",
		"ERROR",
		"FATAL",
	};

	printf("[%s][%s]\n",msg,level_msg[level%5]);
#endif
}

int startup(const char *_ip,int _port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		printf_log(sterror(errno),FATAL);
		exit(2);
	}

	int opt=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_port);
	local.sin_addr.s_addr=inet_addr(ip);

	if(bind(sock,(struct sockaddr_in*)&local,sizeof(local))<0)
	{
		print_log(strerror(errno),FATAL);
		exit(3);
	}

	if(listen(sock,10)<0)
	{
		print_log(strerror(srrno),FATAL);
		exit(4);
	}

	return sock;
}

static int get_line(int sock,char line[],int size)
{
	//一个字符一个字符读
	char c='\0';
	int len=0;
	while(c!='\n' && len < size-1)
	{
		int r=recv(sock,&c,1,0);

		if(r>0) //需要区分'\n'和'\r\n'
		{
			if(c=='\r') //判断下个字符是不是'\n'
			{
				int ret=recv(sock,&c,1,MSG_PEEK);
				if(ret>0)
				{
					if(c=='\n')
						recv(sock,&c,1,0);
					else
						c='\n';
				}
			}
			// \r -> \n  \r\n -> \n
			line[len++]=c;
		}
		else
		{
			c='\n';
		}
	}
	line[len]='\0';
	return len;
}

static void echo_string(int sock)
{}

static int echo_www(int sock,char *path,int size)
{
	int fd=open(path,0_RDONLY);
	if(fd<0)
	{
		echo _string(sock);
		print_log(strerror(errmo),FATAL);
		return 8;
	}

	const char *echo_line="HTTP/1.0 200 OK\r\n";
	send(sock,echo_line,strlen(echo_line),0);
	const char *null_line="\r\n";
	send(sock,null_line,strlen(null_line),0);

	if(sendfile(sock,fd,NULL,size)<0)
	{
		echo_string(sock);
		print_log(strerror(errno),FATAL);
		return 9;
	}

	close(fd);
	return 0;
}

static void drop_header(int sock)
{
	char line[1024];
	int ret=-1;
	do{
		ret=get_line(sock,line,sizeof(line));
	}while(ret>0 && strcmp(line,"\n"));
}

static int exe_cgi(int sock,char)

