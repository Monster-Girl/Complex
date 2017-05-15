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

		recvMsg(msgid,CLIENT_TYPE,buf);
		printf("client # %s\n",buf);
	}
	return 0;
}

