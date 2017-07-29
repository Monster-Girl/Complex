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
#include <string.h> /* bzero */  
#include <netdb.h>  
#include <pthread.h> 
//保存发送包的状态值  
typedef struct pingm_pakcet{  
	struct timeval tv_begin;     //发送时间  
	struct timeval tv_end;       //接收到的时间  
	short seq;                   //序列号  
	int flag;          //1，表示已经发送但是没有接收到回应，0，表示接收到回应  
}pingm_pakcet;  
static pingm_pakcet *icmp_findpacket(int seq);  
static unsigned short icmp_cksum(unsigned char *data, int len);  
static struct timeval icmp_tvsub(struct timeval end, struct timeval begin);  
static void icmp_statistics();  
static void icmp_pack(struct icmp *icmph, int seq, struct timeval *tv,int length);  
static int icmp_unpack(char *buf,int len);  
static void *icmp_recv();  
static void icmp_sigint(int signo);  
static void icmp_usage();  

static pingm_pakcet pingpacket[128];  
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

