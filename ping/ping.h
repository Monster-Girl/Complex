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


void PackIcmp(int seq);
void SendPacket(int sock_icmp,struct sockaddr_in *remote,int num_send);
void RecevePacket(int sock_icmp,struct *local);
void PackIcmp();
int Unpack(struct timeval *ReveceTime);

double GetTime()

#endif
