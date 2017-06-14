/*************************************************************************
	> File Name: alarm.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Tue 13 Jun 2017 04:54:17 AM PDT
 ************************************************************************/

#include<stdio.h>
int main()
{
	alarm(1);
	int count=0;
	while(1)
	{
		printf("count : %d\n",count++);
	}
	return 0;
}
