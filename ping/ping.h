/*************************************************************************
	> File Name: ping.h
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Sun 23 Jul 2017 09:00:21 AM CST
 ************************************************************************/

#ifndef __PING_H_
#define __PING_H_

#include<stdio.h>
#include<netinet/ip_icmp.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<netinet/in.h>  
#include<netinet/ip.h>
#include<pthread.h>
#include<string.h>
#include<signal.h>

struct sockaddr_in dst_addr;
struct sockaddr_in recv_addr;
struct timeval recv_time;
char icmp_pkt[1024]={0};
char recv_pkt[1024]={0};

int sockfd=0;
int bytes=56;
int nsend_pkt=0;
int nrecv_pkt=0;
pid_t pid;

void statistics();
int in_chksum(unsigned short *buf,int size);
double GetRtt(struct timeval *last,struct timeval *first);
int unpack(char *recv_pkt,int size);
void SendPacket();
void RecevePacket();

#endif
