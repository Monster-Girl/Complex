#include"ping.h"

void cal_rtt()
{
	double sum_avg=0;
	int i=0;
	min=max=tmp_rtt[0];
	avg=alltime/nreceive;

	for(i=0;i<nreceive;i++)
	{
		if(tmp_rtt[i]<min)
			min=tmp_rtt[i];
		if(tmp_rtt[i]>max)
			max=tmp_rtt[i];
		
		if((tmp_rtt[i]-avg)<0)
			sum_avg+=avg-tmp_rtt[i];
		else
			sum_avg+=tmp_rtt[i]-avg;
	}
	mdev=sum_avg/nreceive;
}

void tv_sub(struct timeval *recv,struct timeval *send)
{
	long sec=recv->tv_sec - send->tv_sec;
	long usec=recv->tv_usec - send->tv_usec;
	if(usec>=0)
	{
		recv->tv_sec=sec;
		recv->tv_usec=usec;
	}
	else
	{
		recv->tv_sec=sec-1;
		recv->tv_usec=-usec;
	}
}

void statistics()
{
	cal_rtt();
	printf("\n-------%s ping statistics-------\n",dest_addr.sin_addr.s_addr);
	printf("%d packets transmitted,%d received,%d%% packet loss,time %.f ms\n",\
			nsend,nreceive,(nsend-nreceive)/nsend*100,alltime);
	printf("rtt min/avg/max/mdev=%.3f/%.3f/%.3f/%.3f ms\n",\
			min,avg,max,mdev);
	close(sockfd);
	exit(1);
}

unsigned short chksum(unsigned short *add,int len)
{
	unsigned short sum=0;
	while(len>1)   //icmp包头以2个字节进行累加
	{
		sum+=*add++;
		len-=2;
	}

	if(len==1)  //icmp如果还剩下一个字节
	{
		*(unsigned char*)(&sum)+=*(unsigned char*)add;
	}

	sum=(sum>>16)+(sum& 0xffff);
	sum+=(sum>>16);
	return ~sum;
}

void unpack(char *buf,int len)
{
	int iphl=0; //ip头长度
	struct ip *ip;
	struct icmp *icmp;
	struct timeval *send;
	double rtt;

	ip=(struct ip*)buf;
	iphl=(ip->ip_hl)<<2; //求ip报文头长度，ip报头长度*4
	icmp=(struct icmp*)(buf + iphl); //指向icmp报头
	len-=iphl; //icmp报头及数据报的总长度
	if(len<8)
	{
		printf("icmp length is less than 8\n");
		exit(7);
	}

	if((icmp->icmp_type==ICMP_ECHOREPLY)&&(icmp->icmp_id==pid))
	{
		send=(struct timeval *)icmp->icmp_data;
		tv_sub(&recvtime,send);  //接收和发送的时间差
		rtt=recvtime.tv_sec*1000 + recvtime.tv_usec/1000;
		tmp_rtt[nreceive]=rtt;
		alltime+=rtt;  //总时间

		printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.1f ms\n",len,\
				inet_ntoa(src_addr.sin_addr),\
				icmp->icmp_seq,\
				ip->ip_ttl,rtt);
	}
	else
		exit(8);

}

int pack(int nsend)  //打包icmp
{
	int packsize=0;
	struct icmp *icmp;
	struct timeval *time;
	icmp=(struct icmp*)sendpacket;
	icmp->icmp_type=ICMP_ECHO;
	icmp->icmp_type=0;
	icmp->icmp_cksum=0;
	icmp->icmp_seq=nsend; //发送的数据报编号
	icmp->icmp_id=pid;

	packsize=datahead+datalen;  //数据报的大小64字节
	time=(struct timeval*)icmp->icmp_data;
	gettimeofday(time,NULL);//记录发送时间
	icmp->icmp_cksum=chksum((unsigned short*)icmp,packsize);
	return packsize;
}

void send_packet()
{
	int packetsize=0;
	if(nsend < MAX_NO_PACKETS)
	{
		nsend++;
		packetsize=pack(nsend); //打包icmp报头 
		if(sendto(sockfd,sendpacket,packetsize,0,\
					(struct sockaddr*)&dest_addr,\
					sizeof(dest_addr))<0)
		{
			perror("sendto");
			exit(5);
		}
	}
}

void recv_packet()
{
	int recv;
	int addrlen=sizeof(src_addr);
	if(nreceive <nsend)
	{	
		if((recv=recvfrom(sockfd,recvpacket,\
				sizeof(recvpacket),0,\
				(struct sockaddr*)&src_addr,&addrlen))<0)
		{
			perror("recvfrom");
			exit(6);
		}	
		gettimeofday(&recvtime,NULL); //记录接收时间
		unpack(recvpacket,recv);  //分离报头
		nreceive++;
	}
}


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
										
