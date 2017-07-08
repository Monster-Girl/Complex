#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<string.h>
#include <sys/sendfile.h>

int startup(const char* port,int ip);
void print_log(char *msg,int level);
static int get_line(int sock,char line[],int size);
static void echo_string(int sock);
static void drop_header(int sock);
void handler_quest(void* arg);
