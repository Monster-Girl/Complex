#include"comm.h"

static int Msg(int msgflg)
{
	key_t _key=ftok(PATHNAME,PROIJ_ID);
	if(_key<0)
	{
		perror("ftok");
		return -1;
	}

	int msgid=msgget(_key,msgflg);
	if(msgid<0)
	{
		perror("msgget");
		return -2;
	}
	return msgid;
}

int creatMsg()
{
	return Msg(IPC_CREAT|IPC_EXCL|PROIJ_ID);
}

int getMsgQueue()
{
	return Msg(0);
}

int destoryMsg(int msgid)
{
	if(msgctl(msgid,IPC_RMID,NULL)<0)
	{
		perror("msgctl");
		return -1;
	}
	return 0;
}

int sendMsg(int msgid,int who,char* msg)
{
	struct Msgbuf buf;
	buf.mtype=who;
	strcpy(buf.mtext,msg);
	if(msgsnd(msgid,(void*)&buf,sizeof(buf.mtext),0)<0)
	{
		perror("magsnd");
		return -1;
	}
	return 0;
}

int recvMsg(int msgid,int MsgType,char out[])
{
	struct Msgbuf buf;
	if(msgrcv(msgid,(void*)&buf,sizeof(buf),MsgType,0)<0)
	{
		perror("msgrcv");
		return -1;
	}
	strcpy(out,buf.mtext);
	return 0;
}
