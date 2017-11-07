/*************************************************************************
	> File Name: pipe.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Tue 07 Nov 2017 11:13:10 AM CST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
int main()
{
	int pipefd[2];
	int fd=pipe(pipefd);
	if(fd==-1)
	{
		printf("pipe error\n");
		return 1;
	}
	pid_t pid=fork();
	if(pid<0)
	{
		printf("fork error\n");
		return 2;
	}
	else if(pid==0)    //child
	{
		close(pipefd[0]);
		int i=0;
		char* mess=NULL;
		while(i<100)
		{
			mess="hello,i am child\n";
			//printf("%s\n",mess);
			write(pipefd[1],mess,strlen(mess)+1);
			sleep(1);
			i++;
		}
		close(pipefd[1]);
	}
	else   //father
   {
		close(pipefd[1]);
		char msg[100];
		int j=0;
		while(j<100)
		{
			memset(msg,0,sizeof(msg));
			read(pipefd[0],msg,sizeof(msg));
			printf("%s\n",msg);
		//	sleep(1);
			j++;
		}
		close(pipefd[0]);
	}
	return 0;
}
