#include"ping.h"
void PackIcmp(int seq)
{
	struct icmp* picmp;
	struct timeval FirstSendTime;
	picmp->icmp_type=ICMP_ECHO;  //类型为ICMP_ECHO
	picmp->icmp_code=0; 
	picmp->icmp_seq=seq
	picmp->icmp_id=getpid();
	time=(struct timeval*)picmp->icmp_data;
	gettimeofday(&time),0);
	picmp=>icmp_cksum=CheckSum(picmp);
	if(1==seq)
	  FirstSendTime=time;
}



void SendPackeet(int sock_icmp,struct sockaddr_in *remote,int num_send)
{
	PackIcmp(num_send);
	char buf[1024];
	size_t s=sendto(sock_icmp,buf,ICMP_LEN,0,\
		(struct sockaddr*)&remote,sizeof(remote));
	if(s<0)
	{
		perror("sendto");
		return;
	}
}

int RecevePacket(int sock_icmp,\
		struct sockaddr_in *local)
{
	int RecBytes=0;
	struct timeval RecTime;
	char buf[1024];

	RecBytes=recvfrom(sock_icmp,buf,sizeof(buf),\
			(struct sockaddr*)local,sizeof(local));
	if(RecBytes<0)
	{
		perror("recvfrom");
		return 4; 
	}

	gettimeofday(&RecTime,NULL);



}
	
