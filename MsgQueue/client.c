#include"comm.h"

int main()
{
	int msgid=getMsgQueue();
	char buf[1024];
	while(1)
	{
		printf("please enter # ");
		fflush(stdout);
		ssize_t s=read(0,buf,sizeof(buf)-1);
		if(s>0)
		{
			buf[s]=0;
			sendMsg(msgid,SERVER_TYPE,buf);
		}

		if(recvMsg(msgid,CLIENT_TYPE,buf)<0)
			break;
		printf("client # %s\n",buf);
	}
//	destoryMsg(msgid);
	return 0;
}

