#include"ping.h"

static void usage(char* proc)
{
	printf("usage:%s[hostname /IP address]\n",proc);
}

int main(int argc,char *argv[])
{
	if(argc<2)
	{
		usage(argv[0]);
		return 1;
	}

	struct protoent *protocol=NULL;
	int ttl=64;
	struct hostent *host;
	struct in_addr addr;
	int name=-1;
	int size=50*1024;
	pthread_t send_id,recv_id;
	protocol=getprotobyname("icmp");
	if(protocol==NULL)
	{
		printf("unknow proto\n");
		return 2;
	}

	sockfd=socket(AF_INET,SOCK_RAW,protocol->p_proto);
	if(sockfd<0)
	{
		perror("socket");
		return 3;
	}

	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));  //确定缓冲区大小
	//setsockopt(sockfd,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(ttl));
	//setsockopt(sockfd,IPPROTO_IP,IP_TTL,&ttl,sizeof(ttl));


	memset(&dst_addr,0,sizeof(dst_addr));
	dst_addr.sin_family=AF_INET;
	name=inet_aton(argv[1],&addr);
	if(name==0)  //不是ip地址
	{
		host=gethostbyname(argv[1]);
		if(host==NULL)
		{
			printf("address is invalid\n");
			return 4;
		}
		memcpy(&(dst_addr.sin_addr),host->h_addr,sizeof(struct in_addr));
	}
	else
	{
		memcpy(&(dst_addr.sin_addr),&(addr.s_addr),sizeof(struct in_addr));
	}

	pid=getpid();
	printf("PING %s (%s) %d bytes of data.\n",argv[1],\
			inet_ntoa(dst_addr.sin_addr),bytes);
	signal(SIGINT,statistics);

	printf("socket:%d",sockfd);
	name=pthread_create(&send_id,NULL,(void*)SendPacket,NULL);
	printf("sock:%d\n",sockfd);
	if(name!=0)
	{
		perror("pthread_create :send");
		return 5;
	}

	name=pthread_create(&recv_id,NULL,(void*)RecevePacket,NULL);
	if(name!=0)
	{
		perror("pthread_create :recv");
		return 6;
	}
	pthread_join(send_id,NULL);
	pthread_join(recv_id,NULL);
	
	return 0;
}
