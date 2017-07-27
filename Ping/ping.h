#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<netdb.h>


#define MAX_NO_PACKETS 10000
#define PACKET_SIZE 4096


static int sockfd;
static int datalen=56;  //数据长度
static int datahead=8;  //数据报头长
static int nsend=0;   //统计发送
static int nreceive=0; //统计接收
static char sendpacket[PACKET_SIZE];
static char recvpacket[PACKET_SIZE];
struct sockaddr_in dest_addr; //目的地址
struct sockaddr_in src_addr;  //初始地址
struct timeval recvtime;
static double tmp_rtt[MAX_NO_PACKETS];

static double alltime=0;
static double min=0;
static double max=0;
static double avg=0;
static double mdev=0;
pid_t pid;

void statistics();
void cal_rtt();
void tv_sub(struct timeval *recv,struct timeval *send); //接收和发送时间差
int pack(int nsend); //设置报头
void unpack(char *buf,int len); //分离报头
void send_packet();  //发送报文
void recv_packet();  //接收报文
