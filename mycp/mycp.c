/*************************************************************************
	> File Name: mycp.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Thu 15 Jun 2017 02:42:41 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>

int buf[64];
sem_t blacksem;
sem_t datasem;


void consume(void* arg)
{
	int step=0;
	while(1)
	{
		sem_wait(&datasem);
		int data=buf[step];
		printf("consumer : %d\n",data);
		step++;
		step%=64;
		sem_post(&blacksem);
	}
}


void product(void* arg)
{
	int step=0;
	while(1)
	{
		sem_wait(&blacksem);
		int data=rand()%1234;
		buf[step]=data;
		printf("product : %d\n",data);
		step++;
		step%=64;
		sem_post(&datasem);
		sleep(1);
	}
}


int main()
{

	sem_init(&blacksem,0,64);
	sem_init(&datasem,0,0);
	pthread_t productor,consumer;

	pthread_create(&consumer,NULL,consume,NULL);
	pthread_create(&productor,NULL,product,NULL);

	pthread_join(productor,NULL);
	pthread_join(consume,NULL);

	return 0;
}
