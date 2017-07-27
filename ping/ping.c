#include"ping.h"

void statistics()
{
	printf("\n---%s ping statistics---\n",inet_ntoa(dst_addr.sin_addr));
	printf("%d packets transmittede, %d received, %.2f%c packet loss\n",\
			nsend_pkt,nrecv_pkt,\
			(float)100*(nsend_pkt-nrecv_pkt)/nsend_pkt,'%');
	close(sockfd);
	exit(0);
}

int in_chksum(unsigned short *buf,int size)
{
	int len=size;
	int sum=0;
	unsigned short *str=buf;
	while(len>1)
	{
		sum+=*str++;
		len-=2;
	}
	if(len==1)
	{
		sum+=*(unsigned char*)str;
	}

	// 将32位的高16位与低16位相加
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned char)~sum;
}

int PackIcmp(int send_pkt)
{
	struct icmp *pkt=(struct icmp*)icmp_pkt;
	struct timeval *pTime=NULL;
	pkt->icmp_type=ICMP_ECHO;
	pkt->icmp_code=0;
	pkt->icmp_seq=htons(send_pkt);
	pkt->icmp_id=getpid();
	pTime=(struct timeval*)pkt->icmp_data;
	gettimeofday(pTime,NULL);
	pkt->icmp_cksum=in_chksum((unsigned short*)pkt,bytes+8);
	return bytes+8;
}

double GetRtt(struct timeval *last,struct timeval *first)
{
	double rtt;
	if((last->tv_usec-=first->tv_usec)<0)
	{
		--last->tv_sec;
		last->tv_usec+=1000000;
	}
	last->tv_sec-=first->tv_sec;
	rtt=last->tv_sec * 1000 + (double)last->tv_usec/(double)1000;
	return rtt;
}

int unpack(char *recv_pkt,int size)
{
	struct ip *ip=NULL;
	int iphl;
	struct icmp *icmp;
	struct timeval *send;
	double rtt;
	
	ip=(struct ip*)recv_pkt;
	iphl=ip->ip_hl;  //ip的首部长度为4个字节
	icmp=(struct icmp*)(recv_pkt+iphl); //越过ip报文的首部，指向icmp报文	
	size-=iphl;

	if(size<8)
	{
		printf("ICMP size is less than 8\n");
		return -1;
	}
	
	if((icmp->icmp_type==0) && (icmp->icmp_id=pid)) //判断报文是否是我们发送的

	{
		send=(struct timeval*)icmp->icmp_data;
		rtt=GetRtt(&recv_time,send);//计算往返时间
		printf("%d bytes from %s : icmp_seq=%d ttl=%d time=%.3f ms\n",\
			size,inet_ntoa(recv_addr.sin_addr),\
			ntohs(icmp->icmp_seq),ip->ip_ttl,rtt);
	}
	return 0;
}

void SendPacket()
{
	int send_bytes=0;
	int ret=-1;
	while(1)
	{
		nsend_pkt++;
		send_bytes=PackIcmp(nsend_pkt);	
		printf("socket11:%d\n",sockfd);
		ret=sendto(sockfd,icmp_pkt,send_bytes,0,\
				(struct sockaddr*)&dst_addr,sizeof(dst_addr));
		if(ret==-1)
		{
			//perror("sendto");
			printf("send fail\n");
			sleep(1);
			//continue;
			break;
		}
		sleep(1);
	}
}

void RecevePacket()
{
	fd_set rd_set;
	struct timeval time;
	time.tv_sec=5;
	time.tv_usec=0;
	int ret=0;
	int nread=0;
	socklen_t recv_len=sizeof(recv_addr);

	while(1)
	{
		FD_ZERO(&rd_set);
		FD_SET(sockfd,&rd_set);
		ret=select(sockfd+1,&rd_set,NULL,NULL,&time);
		if(ret<=0)
		{
			continue;
		}
		else if(FD_ISSET(sockfd,&rd_set))
		{
			nread=recvfrom(sockfd,recv_pkt,sizeof(recv_pkt),0,\
					(struct sockaddr*)&recv_addr,&recv_len);
			if(nread<0)
			{
				continue;
			}
			gettimeofday(&recv_time,NULL);

			if(unpack(recv_pkt,nread)==-1)
				continue;
		}
	}
}

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
