#include"ping.h"

static void usage(char *proc)
{
	printf("%s[hostname/ip address]",proc);
}

int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		usage(argv[0]);
		return 1;
	}

	struct hostent *host;
	struct protoent *protocol;
	int waittime=MAX_NO_PACKETS;
	int size=50*1024;

	protocol=getprotobyname("icmp");//获取协议
	if(protocol==NULL)
	{
		perror("getprotobyname");
		return 2;
	}

	sockfd=socket(AF_INET,SOCK_RAW,protocol->p_proto); //生成icmp原始套接字，只有root可以生成
	if(sockfd<0)
	{
		perror("socket");
		return 3;
	}

	setuid(getuid());  //回收root权限，重新设置权限

	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)); 
	//在这里扩大套接字的接收缓冲区，减少接收缓冲区溢出的可能性，若不小心ping到一个广播地址或者多播地址，将会收到大量应答
	
	bzero(&dest_addr,sizeof(dest_addr)); //初始化dest_addr

	dest_addr.sin_family=AF_INET;

	if(inet_addr(argv[1])==INADDR_NONE)  //判断是主机名还是ip地址
	{
		if((host=gethostbyname(argv[1]))==NULL)  //是主机名
		{
			perror("gethostbyname");
			return 4;
		}
	
		memcpy((char*)&dest_addr.sin_addr,host->h_addr,host->h_length);
	}
	else  //是ip地址
	{
		dest_addr.sin_addr.s_addr=inet_addr(argv[1]);
	}

	pid=getpid();  //得到icmp的id信息

	printf("PING %s(%s): %d bytes of data.\n",\
			argv[1],inet_ntoa(dest_addr.sin_addr),datalen);

	signal(SIGINT,statistics);
	while(nsend<MAX_NO_PACKETS)
	{
		sleep(1);
		send_packet();
		recv_packet();
	}
}

