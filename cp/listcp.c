/*************************************************************************
	> File Name: listcp.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Mon 12 Jun 2017 12:38:11 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
typedef struct node
{
	int data;
	struct node* next;
}node_t,*node_p,**node_pp;

node_p head=NULL;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


node_p AllocNode(int d,node_p node)
{
	node_p n=(node_p)malloc(sizeof(node_t));
	if(!n)
	{
		perror("malloc");
		exit(1);
	}

	n->data=d;
	n->next=node;
	return n;

}

void InitList(node_pp _h)
{
	*_h=AllocNode(0,NULL);
}

int IsEmpty(node_p n)
{
	return n->next==NULL?1:0;
}

void FreeNode(node_p n)
{
	if(n!=NULL)
	{
		free(n);
		n=NULL;
	}
}

void PushFront(node_p n,int d)
{
	node_p tmp=AllocNode(d,NULL);
	tmp->next=n->next;
	n->next=tmp;
}

void PopFront(node_p n,int* out)
{
	if(!IsEmpty(n))
	{
		node_p tmp=n->next;
		n->next=tmp->next;
		*out=tmp->data;
		FreeNode(tmp);
	}
}

void ShowList(node_p n)
{
	node_p begin=n->next;
	while(begin)
	{
		printf("%d ",begin->data);
		begin=begin->next;
	}
	printf("\n");
}

void Destory(node_p n)
{
	int data;
	while(!IsEmpty(n))
	{
		PopFront(n,&data);
	}
	FreeNode(n);
}

void* Consumer(void* arg)
{
	int data=0;
	while(1)
	{
		pthread_mutex_lock(&lock);
		while(IsEmpty(head))
		{
			pthread_cond_wait(&cond,&lock);
		}
		PopFront(head,&data);
		printf("consumer done: %d\n",data);
		pthread_mutex_unlock(&lock);
	}
}

void* Product(void* arg)
{
	int data=0;
	while(1)
	{
		pthread_mutex_lock(&lock);
		data=rand()%1234;
		PushFront(head,data);
		printf("productor done:%d\n",data);
		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&cond);
		sleep(1);
	}
}

int main()
{
	pthread_mutex_init(&lock,NULL);

	InitList(&head);
	pthread_t consumer,productor;
	pthread_create(&consumer,NULL,Consumer,NULL);
	pthread_create(&productor,NULL,Product,NULL);

	pthread_join(consumer,NULL);
	pthread_join(productor,NULL);

	Destory(head);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
	return 0;
}
