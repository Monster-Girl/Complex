/*************************************************************************
	> File Name: bar.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Wed 28 Jun 2017 04:47:51 PM CST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>

void mybar()
{
	char bar[101]={'\0'};
	int i=0;
	char str[]={'|','/','-','\\'};

	printf("\033[?251\033[41m");
	fflush(stdout);
	printf("\033[0m");

	while(i<=100)
	{
		printf("\033[42m\033[1m");
		printf("[%c]%s%3d%% \r\033[0m",str[i%4],bar,i);
		fflush(stdout);
		usleep(30000);
		bar[i]=' ';
		i++;
	}
	printf("\n\033[?25h");
}

int main()
{
	mybar();
	return 0;
}
