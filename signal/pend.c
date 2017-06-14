/*************************************************************************
	> File Name: pend.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Wed 14 Jun 2017 01:05:16 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void printSigset(sigset_t *pending)
{
	int i=0;
	for(;i<32;i++)
	{
		if(sigismember(pending,i))
		{
			printf("1");
		}
		else
			printf("0");
	}
	printf("\n");
}

int main()
{
	sigset_t set,oset,pending;
	sigemptyset(&set);
	sigemptyset(&oset);
	sigaddset(&set,2);

	sigprocmask(SIG_SETMASK,&set,&oset);
	while(1)
	{
		sigpending(&pending);
		printSigset(&pending);
		sleep(1);
	}
	return 0;
}
