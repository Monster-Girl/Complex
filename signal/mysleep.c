/*************************************************************************
	> File Name: mysleep.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Wed 14 Jun 2017 02:59:32 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void handler(int sig)
{
}

int mysleep(int timeout)
{
	struct sigaction act,oact;
	act.sa_handler=handler;
	act.sa_flags=0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM,&act,&oact);
	alarm(timeout);
	pause();
	int ret=alarm(0);
	sigaction(SIGALRM,&oact,NULL);
	return ret;
}

int main()
{
	while(1)
	{
		mysleep(2);
		printf("2 seconds!\n");
	}
	return 0;
}
