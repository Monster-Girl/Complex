/*************************************************************************
	> File Name: ping.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Fri 28 Jul 2017 04:53:00 PM CST
 ************************************************************************/
#include"ping.h"

void icmp_statistics()
{
	long time=(tv_interval.tv_sec *1000)+(tv_interval.tv_usec/1000);
	cal_rtt();
	printf("---%s ping statistics---\n",dest_str);
	printf("%d packets transmitted, %d received, %d%% packet loss,time %ld ms.\n",\
			packet_send,packet_recv,(packet_send-packet_recv)*100/packet_send,time);
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",min,avg,max,mdev);

	FILE *fp;
	fp=fopen("out.txt","w");
	fprintf(fp,"%d %.3f\n",(packet_send-packet_recv)*100/packet_send,avg);
	fclose(fp);

}

void cal_rtt()
{
	double sum_avg=0;
	int i=0;
	min=max=tmp_rtt[0];
	avg=all_time/(double)packet_recv;
	for(i=0;i<(double)packet_recv;i++)
	{
		if(tmp_rtt[i]<min)
			min=tmp_rtt[i];
		if(tmp_rtt[i]>max)
			max=tmp_rtt[i];

		if((tmp_rtt[i]-avg)>0)
			sum_avg+=tmp_rtt[i]-avg;
		else
			sum_avg+=avg-tmp_rtt[i];
	}
	mdev=sum_avg/packet_recv;
}

void icmp_sigint(int signo)
{
	alive=0;
	gettimeofday(&tv_end,NULL);
	tv_interval=icmp_tvsub(tv_end,tv_begin);
}

pingm_packet *icmp_findpacket(int seq)
{
	int i=0;
	pingm_packet *found=NULL;
	if(seq==-1)   
	{
		for(i=0;i<128;i++)
		{
			if(pingpacket[i].flag==0)
			{
				found=&pingpacket[i];
				break;
			}
		}
	}
	else if(seq>=0)
	{
		for(i=0;i<128;i++)
		{
			if(pingpacket[i].seq==seq)
			{
				found=&pingpacket[i];
				break;
			}
		}
	}
	return found;
}


struct timeval icmp_tvsub(struct timeval end,struct timeval begin)
{
	struct timeval tv;
	tv.tv_sec=end.tv_sec-begin.tv_sec;
	tv.tv_usec=end.tv_usec-begin.tv_usec;

	if(tv.tv_usec<0)
	{
		tv.tv_sec-=1;
		tv.tv_usec+=1000000;
	}
	return tv;
}



unsigned short icmp_cksum(unsigned char* data,int len)
{
	int sum=0;
	int old=len & 0x01;  //判断是否为奇数
	while(len & 0xfffe)  //将数据按照2个字节为单位累加
	{
		sum+=*(unsigned short*)data;
		data+=2;
		len-=2;
	}

	if(old)   //如果是奇数，则需要对最后的一个数据进行处理
	{
		unsigned short tmp=((*data)<<8)&0xff00;
		sum+=tmp;
	}

	sum=(sum>>16)+(sum & 0xffff);  //将高低位相加
	sum+=(sum>>16);  //将最高位相加

	return ~sum;
}

void icmp_pack(struct icmp *icmp,int seq,struct timeval *tv,int length)
{
	unsigned char i=0;
	icmp->icmp_type=ICMP_ECHO;
	icmp->icmp_code=0;
	icmp->icmp_cksum=0;
	icmp->icmp_seq=seq;
	icmp->icmp_id=pid & 0xffff;
	for(i=0;i<length;i++)
	{
		icmp->icmp_data[i]=i;
	}

	icmp->icmp_cksum=icmp_cksum((unsigned char*)icmp,length);

//	printf("send: type: %d, code: %d, sum: %d, seq: %d, id: %d\n", \
			icmp->icmp_type, icmp->icmp_code, icmp->icmp_cksum, \
			icmp->icmp_seq, icmp->icmp_id);
//	fflush(stdout);
}


int icmp_unpack(char *buf,int len)
{
	int i=0;
	int iphl=0;
	struct ip *ip=NULL;
	struct icmp *icmp=NULL;
	int rtt=0;

	ip=(struct ip*)buf;
	iphl=ip->ip_hl<<2;
	icmp=(struct icmp*)(buf+iphl);
	len-=iphl;
	if(len<8)
	{
		printf("icmp packet length is less than 8\n");
		return -1;
	}

//	printf("recv: type: %d, code: %d, sum: %d, seq: %d, id: %d\n",\
			icmp->icmp_type, icmp->icmp_code, icmp->icmp_cksum,\
			icmp->icmp_seq, icmp->icmp_id);

//	fflush(stdout);
	if((icmp->icmp_type==ICMP_ECHOREPLY)&&(icmp->icmp_id==pid))
	{
		struct timeval tv_interval;
		struct timeval tv_send;
		struct timeval tv_recv;

		pingm_packet *packet=icmp_findpacket(icmp->icmp_seq);
		if(packet==NULL)
			return -1;
	
		packet->flag=0;
		tv_send=packet->tv_begin;

		gettimeofday(&tv_recv,NULL);
		tv_interval=icmp_tvsub(tv_recv,tv_send);
		rtt=tv_interval.tv_sec*1000+tv_interval.tv_usec/1000;

		tmp_rtt[packet_recv]=rtt;
		all_time+=rtt;
		packet_recv++;
		printf("%d byte from %s:icmp_seq=%u ttl=%d rtt=%d ms\n",len,inet_ntoa(ip->ip_src),icmp->icmp_seq,ip->ip_ttl,rtt);
	}
	else
		return -1;
}

void *icmp_send()
{
	gettimeofday(&tv_begin,NULL);
	while(alive)
	{
		int size=0;
		struct timeval tv;
		gettimeofday(&tv,NULL);  //当前包发送的时间
		pingm_packet *packet=icmp_findpacket(-1);
		if(packet)
		{
			packet->seq=packet_send;
			packet->flag=1;
			gettimeofday(&packet->tv_begin,NULL);
		}
		icmp_pack((struct icmp*)send_buff,packet_send,&tv,64);

		size=sendto(rawsock,send_buff,64,0,(struct sockaddr*)&dest,sizeof(dest));
		if(size<0)
		{
			perror("sendto");
			continue;
		}
		packet_send++;
		sleep(1);
	}
}

void *icmp_recv()
{
	struct timeval tv;
	tv.tv_usec=200;
	tv.tv_sec=0;
	fd_set readfd;

	while(alive)
	{
		int ret=0;
		FD_ZERO(&readfd);
		FD_SET(rawsock,&readfd);
		ret=select(rawsock+1,&readfd,NULL,NULL,&tv);
		//select用途是为了管理tv，每发一次请求75秒
		//我们只是判断是否在，所以自己设置了时间
		//用select来管理
		switch(ret)
		{
			case -1: //发生错误
				break;
			case 0:  //超时
				break;
			default:  //收到一个包
				{
					int fromlen=0;
					struct sockaddr from;
					int size=recv(rawsock,recv_buff,sizeof(recv_buff),0);
					if(errno==EINTR)
					{
						perror("recv");
						continue;
					}
					ret=icmp_unpack(recv_buff,size);
					if(ret==-1)
						continue;
				}
		break;
		}
	}
}


static void usage(char *proc)
{
	printf("%s[hostname | ip adress]",proc);
}

int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		usage(argv[0]);
		return 1;
	}
	struct hostent *host=NULL;
	struct protoent *protocol=NULL;
	
	int size=128*K;
	protocol=getprotobyname("icmp");

	if(protocol==NULL)
	{
		perror("getprotobyname");
		return 2;
	}

	memcpy(dest_str,argv[1],strlen(argv[1])+1);
	memset(pingpacket,0,sizeof(pingm_packet)*128);
	
	rawsock=socket(AF_INET,SOCK_RAW,protocol->p_proto);
	if(rawsock<0)
	{
		perror("socket");
		return 3;
	}

	pid=getpid();
	setsockopt(rawsock,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)); //增大接收缓冲区
	bzero(&dest,sizeof(&dest));
	dest.sin_family=AF_INET;

	unsigned long  inaddr=inet_addr(argv[1]);
	if(inaddr==INADDR_NONE)
	{
		host=gethostbyname(argv[1]);
		if(host==NULL)
		{
			perror("gethostname");
			return 4;
		}
		memcpy((char*)&dest.sin_addr,host->h_addr,host->h_length);
	}
	else
	{
		memcpy((char*)&dest.sin_addr,&inaddr,sizeof(inaddr));
	}

	inaddr=dest.sin_addr.s_addr;
	printf("PING %s 56(84) bytes of data.\n",dest_str);

	signal(SIGINT,icmp_sigint);

	alive=1;
	pthread_t send_id,recv_id;
	int err=0;
	err=pthread_create(&send_id,NULL,icmp_send,NULL);
	if(err<0)
	{
		return 5;
	}

	err=pthread_create(&recv_id,NULL,icmp_recv,NULL);
	if(err<0)
	{
		return 6;
	}

	pthread_join(send_id,NULL);
	pthread_join(recv_id,NULL);

	close(rawsock);
	icmp_statistics();
	return 0;
}
