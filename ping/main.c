#include"ping.h"

static void usage(char* proc)
{
	printf("usage:%s[hostname /IP address]\n",proc);
}

int main(int agrc,char *argv[])
{
	if(argc<2)
	{
		usage(argv[0]);
		return 1;
	}

	int sock_icmp=socket(AF_INET,SOCK_RAM,IPPROTO_ICMP); //生成原始套接字
	if(sock_fd<0)
	{
		perror("socket");
		return 2;
	}

	struct sockaddr_in remote;
	remote.sin_family=AF_INET; //设置目的地址信息

	in_addr_t inaddr=inet_addr(argv[1]);
	struct hostent *host;
	//判断是主机名还是ip地址
	if(inaddr==INADDR_NONE)  //如果转换为点分十进制出错，则为主机名	
	{
		host=gethostbyname(argv[1]);
		if(host==NULL)
		{
			perror("gethostbyname");
			return 3;
		}
		remote.sin_addr=*(struct sockaddr*)host->h_addr_list;
	}
	else  //是ip地址
	{
		remote.sin_addr=inaddr;
	}

	//输出主机名/ip地址信息
	printf("PING %s (%s) ：%d bytes of data.\n",argv[1],\
			inet_ntoa(remote.sin_addr,ICMP_SIZE);
	
	//发送报文、接收报文
	int num_send=0;  //用来统计发送的次数
	strcut icmp *sendicmp;

	while(1)
	{
	   num_send++;
	   //发送报文
	   SendPacket(sock_icmp,&remote,num_send);

	   //接收报文
	   RecevePacket(sock_icmp,&remote);

	}
	
}
