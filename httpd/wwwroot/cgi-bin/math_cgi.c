/*************************************************************************
	> File Name: math_cgi.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Sat 05 Aug 2017 04:32:19 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
void mymath(char *query_string)
{
	//data1=100&data2=200
	char *argv[3];
	int i=0;
	char *start=query_string;
	while(*start)
	{
		if(*start=='=')
		{
			start++;
			argv[i++]=start;
			continue;
		}
		if(*start=='&')
			*start='\0';
		start++;
	}
	argv[i]=NULL;
	int data1=atoi(argv[0]);
	int data2=atoi(argv[1]);
	printf("%d + %d = %d<br/>", data1, data2, data1 + data2);
	printf("%d - %d = %d<br/>", data1, data2, data1 - data2);
	printf("%d * %d = %d<br/>", data1, data2, data1 * data2);
	printf("%d / %d = %d<br/>", data1, data2, data2==0? 0 : data1 / data2);
	printf("%d %% %d = %d<br/>", data1, data2, data2==0? 0 : data1 % data2);
}


int main()
{
	printf("math_cgi\n");
	char *method=getenv("METHOD");
	char *query_string=NULL;
	int content_len=-1;
	char buf[1024];
	if(method)
	{
		if(strcasecmp(method,"GET")==0)
		{
			query_string=getenv("QUERY_STRING");
		}
		else
		{
			content_len=atoi(getenv("CONTENT_LEN"));
			int i=0;
			for(;i<content_len;i++)
				read(0,&buf[i],1);
			buf[i]=0;
			query_string=buf;
		}
	}
	mymath(query_string);
	return 0;
}
