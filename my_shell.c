/*************************************************************************
	> File Name: my_shell.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Fri 12 May 2017 06:37:42 PM PDT
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
int main()
{
	while(1)
	{
		printf("[myshell@myhost test]# ");
		fflush(stdout);

		char buf[1024];
		ssize_t s=read(0,buf,sizeof(buf)-1);
		if(s>0)
		{
			buf[s-1]=0;
			//printf("%s\n",buf);
		}

		char *_argv[32];
		_argv[0]=buf;
		int i=1;
		char *start=buf;
		while(*start)
		{
			if(*start==' ')
			{
				* start=0;
				start++;
				_argv[i++]=start;
			}
			else
				start++;
		}
		_argv[i]=NULL;

		pid_t id=fork();
		if(id==0)
		{
			int j=0;
			int fd;
			for(j=0;j<=i;j++)
			{


				if(strcmp(_argv[j],">")==0)
				{
			
					char* file=_argv[j+1];
				
					_argv[j]=NULL;
					close(1);
					fd=open(file,O_WRONLY|O_CREAT ,0666);
				
					break;

				}
			}
			execvp(_argv[0],_argv);
			exit(1);
		}
		else
		{
			int status=0;
			pid_t ret=waitpid(id,&status,0);
			if(ret>0)
			{

				if(WIFEXITED(status))
				{
				  printf("exitCode:%d\n",WEXITSTATUS(status));
				}

				else

					printf("child ,quit by sig!\n");
			}
		}
	}
}

