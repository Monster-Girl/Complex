/*************************************************************************
	> File Name: ping.h
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Fri 28 Jul 2017 04:19:09 PM CST
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
#include <string.h> 
#include <netdb.h>  
#include <pthread.h> 

//保存发送包的状态值  
typedef struct pingm_packet
{  
	struct timeval tv_begin;     //发送时间  
	struct timeval tv_end;       //接收到的时间  
	short seq;                   //序列号  
	int flag;          //1:表示已经发送但是没有接收到回应，0:表示接收到回应  
}pingm_packet;  

#define K 1024 
#define BUFFSIZE 72
#define MAX 10000
static pingm_packet pingpacket[128];  
static unsigned char send_buff[BUFFSIZE];        
static unsigned char recv_buff[2*K];             //防止接收溢出，设置大一些  
static struct sockaddr_in dest;                  //目的地址  
static int rawsock = 0;                          //发送和接收线程需要的socket描述符  
static pid_t pid;                                //进程PID  
static int alive = 0;                            //是否接收到退出信号  
static short packet_send = 0;                    //已经发送的数据包数量				
static short packet_recv = 0;                    //已经接收的数据包数量				
static char dest_str[80];                        //目的主机字符串  
static struct timeval tv_begin, tv_end, tv_interval; 
static double tmp_rtt[MAX];      //用来保存时间
static double all_time=0;          //总的时间
static double min=0;             //最短时间
static double max=0;             //最大时间
static double avg=0;             //平均时间
static double mdev=0;            //算术平均差时间

pingm_packet *icmp_findpacket(int seq);  
unsigned short icmp_cksum(unsigned char *data, int len);  
struct timeval icmp_tvsub(struct timeval end, struct timeval begin);  
void icmp_statistics();  
void icmp_pack(struct icmp *icmph, int seq, struct timeval *tv,int length);  
int icmp_unpack(char *buf,int len); 
void *icmp_send();
void *icmp_recv();  
void icmp_sigint(int signo);  
void cal_rtt();

