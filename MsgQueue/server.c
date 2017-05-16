#include"comm.h"

int main()
{
	int msgid=creatMsg();
	char buf[1024];
	while(1)
	{
		//buf[0]=0;
		if(recvMsg(msgid,CLIENT_TYPE,buf)<0)
			break;
		printf("client # %s\n",buf);
		fflush(stdout);
	//	ssize_t s=read(0,buf,sizeof(buf)-1);
	//	if(s>0)
	//	{
		//	buf[s]=0;
			sendMsg(msgid,SERVER_TYPE,buf);
		//}
	}

	destoryMsg(msgid);
	return 0;
}
