#ifndef __COMM_H__
#define __COMM_H__


#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<time.h>
#include<string.h>
#include<sys/msg.h>

#define PATHNAME "."
#define PROJ_ID 0666
#define SERVER_TYPE 1
#define CLIENT_TYPE 2

struct Msgbuf
{
	int mtype;
	char mtext[1024];
};

static int Msg(int msgflg);
int creatMsg();
int sendMsg(int msgid,int who,char* msg);
int recvMsg(int msgid,int MsgType,char out[]);
int destoryMsg();
int getMsgQueue();

#endif
