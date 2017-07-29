/*************************************************************************
	> File Name: ping.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Fri 28 Jul 2017 04:53:00 PM CST
 ************************************************************************/
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netinet/ip.h>  
#include <netinet/ip_icmp.h>  
#include <unistd.h>  
#include <signal.h>  
#include <arpa/inet.h>  
#include <errno.h>  
#include <sys/time.h>  
#include <stdio.h>  
#include <string.h> /* bzero */  
#include <netdb.h>  
#include <pthread.h> 
//保存发送包的状态值  
typedef struct pingm_packet
{  
	struct timeval tv_begin;     //发送时间  
	struct timeval tv_end;       //接收到的时间  
	short seq;                   //序列号  
	int flag;          //1，表示已经发送但是没有接收到回应，0，表示接收到回应  
}pingm_packet;  

static pingm_packet *icmp_findpacket(int seq);  
static unsigned short icmp_cksum(unsigned char *data, int len);  
static struct timeval icmp_tvsub(struct timeval end, struct timeval begin);  
static void icmp_statistics();  
static void icmp_pack(struct icmp *icmph, int seq, struct timeval *tv,int length);  				
static int icmp_unpack(char *buf,int len); 
static void *icmp_send();
static void *icmp_recv();  
static void icmp_sigint(int signo); 
static void icmp_usage();  


static pingm_packet pingpacket[128];  
#define K 1024  
#define BUFFERSIZE 72                            //发送缓冲区的大小  
static unsigned char send_buff[BUFFERSIZE];        
static unsigned char recv_buff[2*K];             //防止接收溢出，设置大一些  
static struct sockaddr_in dest;                  //目的地址  
static int rawsock = 0;                          //发送和接收线程需要的socket描述符  
static pid_t pid;                                //进程PID  
static int alive = 0;                            //是否接收到退出信号  
static short packet_send = 0;                    //已经发送的数据包数量  			
static short packet_recv = 0;                    //已经接收的数据包数量  			
static char dest_str[80];                        //目的主机字符串  
static struct timeval tv_begin, tv_end, tv_interval; 




void icmp_statistics()
{
	long time=(tv_interval.tv_sec *1000)+(tv_interval.tv_usec/1000);
	printf("---%s ping statistics---\n",dest_str);
	printf("%d packets transmitted,%d received,%d%c packet loss,time %ld ms.\n",\
			packet_send,packet_recv,(packet_send-packet_recv)*100/packet_send,'%',time);
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
	return found;
}


struct timeval icmp_tvsub(struct timeval end,struct timeval begin)
{
	struct timeval tv;
	tv.tv_sec=end.tv_sec-begin.tv_sec;
	tv.tv_usec=end.tv_sec-begin.tv_usec;

	if(tv.tv_usec<0)
	{
		tv.tv_sec--;
		tv.tv_usec+=1000000;
	}
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
	icmp->icmp_id=pid;
	for(i=0;i<length;i++)
	{
		icmp->icmp_data[i]=i;
	}

	icmp->icmp_cksum=icmp_cksum((unsigned char*)icmp,length);
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

		rtt=tv_interval.tv_sec *1000+tv_interval.tv_usec/1000;

		printf("%d byte from %s:icmp_seq=%u ttl=%d rtt=%d ms\n",len,inet_ntoa(ip->ip_src),icmp->icmp_seq,ip->ip_ttl,rtt);
		packet_recv++;	
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
		switch(ret)
		{
			case -1: //发生错误
				break;
			case 0:  //超时
				break;
			default:  //收到一个包
				{
	
					int fromlem=0;
					struct sockaddr from;

					int size=recv(rawsock,recv_buff,sizeof(recv_buff),0);
					if(errno==EINTR)
					{
						perror("recv");
						continue;
					}

					ret=icmp_unpack(recv_buff,size);
					if(ret==1)
						continue;
				}
		}
		break;
	}
}


static void usage(char *proc)
{
	printf("%s[hostname|ip adress]",proc);
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
	memset(pingpacket,0,sizeof(pingpacket)*128); //??

	rawsock=socket(AF_INET,SOCK_RAW,protocol->p_proto);
	if(rawsock<0)
	{
		perror("socket");
		return 3;
	}

	pid=getuid();

	setsockopt(rawsock,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)); //增大接收缓冲区
	bzero(&dest,sizeof(&dest));
	dest.sin_family=AF_INET;

	int inaddr=inet_addr(argv[1]);
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
