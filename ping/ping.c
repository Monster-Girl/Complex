#include"ping.h"

void statistics()
{
	printf("\n---%s ping statistics---\n",inet_ntoa(dst_addr.sin_addr));
	printf("%d packets transmittede, %d received, %.3f%c packet loss\n",\
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
	printf("1\n");
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
	
	if(!(icmp->icmp_type==0) || (icmp->icmp_id!=getpid())) //判断报文是否是我们发送的
	{
		printf("the icmp packet is not send by us\n");
		return -1;
	}

	send=(struct timeval*)icmp->icmp_data;
	rtt=GetRtt(&recv_time,send);//计算往返时间
	
	
	printf("%d bytes from %s : icmp_seq=%d ttl=%d time=%.3f ms\n",\
			size,inet_ntoa(recv_addr.sin_addr),\
			ntohs(icmp->icmp_seq),ip->ip_ttl,rtt);
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
		ret=sendto(sockfd,icmp_pkt,send_bytes,0,\
				(struct sockaddr*)&dst_addr,sizeof(dst_addr));
		if(ret==-1)
		{
			printf("send fail\n");
			sleep(1);
			continue;
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
