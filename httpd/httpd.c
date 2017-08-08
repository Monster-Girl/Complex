/*************************************************************************
	> File Name: httpd.c
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Wed 05 Jul 2017 05:04:57 PM CST
 ************************************************************************/

#include"httpd.h"
void print_log(char *msg,int level)
{
#ifdef _STDOUT_
	const char *const level_msg[]={
		"SUCCESS",
		"NOTICE",
		"WARNING",
		"ERROR",
		"FATAL",
	};

	printf("[%s][%s]\n",msg,level_msg[level%5]);
#endif
}

int startup(const char *_ip,int _port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		print_log(strerror(errno),FATAL);
		exit(2);
	}

	int opt=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_port);
	local.sin_addr.s_addr=inet_addr(_ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		print_log(strerror(errno),FATAL);
		exit(3);
	}

	if(listen(sock,10)<0)
	{
		print_log(strerror(errno),FATAL);
		exit(4);
	}

	return sock;
}

static int get_line(int sock,char line[],int size)
{
	//一个字符一个字符读
	char c='\0';
	int len=0;
	while(c!='\n' && len < size-1)
	{
		int r=recv(sock,&c,1,0);

		if(r>0) //需要区分'\n'和'\r\n'
		{
			if(c=='\r') //判断下个字符是不是'\n'
			{
				int ret=recv(sock,&c,1,MSG_PEEK);
				if(ret>0)
				{
					if(c=='\n')
						recv(sock,&c,1,0);
					else
						c='\n';
				}
			}
			// \r -> \n  \r\n -> \n
			line[len++]=c;
		}
		else
		{
			c='\n';
		}
	}
	line[len]='\0';
	return len;
}

static void echo_string(int sock)
{}

static int echo_www(int sock,char *path,ssize_t size)
{
	int fd=open(path,O_RDONLY);
	if(fd<0)
	{
		echo_string(sock);
		print_log(strerror(errno),FATAL);
		return 8;
	}

	const char *echo_line="HTTP/1.0 200 OK\r\n";
	send(sock,echo_line,strlen(echo_line),0);
	const char *null_line="\r\n";
	send(sock,null_line,strlen(null_line),0);

	if(sendfile(sock,fd,NULL,size)<0)
	{
		echo_string(sock);
		print_log(strerror(errno),FATAL);
		return 9;
	}

	close(fd);
	return 0;
}

static void drop_header(int sock)
{
	char line[1024];
	int ret=-1;
	do{
		ret=get_line(sock,line,sizeof(line));
	}while(ret>0 && strcmp(line,"\n"));
}

static int exe_cgi(int sock,const char *method,\
		const char *path,const char *query_string)
{
	int content_len=-1;  
	printf("hhhhh%s",method);
	if(strcasecmp(method,"GET")==0)  
	{
		printf("hello");
		drop_header(sock);
		printf("wwww%s",query_string);
	}
	else   //POST ---> 开始处理post的报文信息
	{
		char line[1024];
		int ret=-1;
		do{
			ret=get_line(sock,line,sizeof(line));
			//content-length指明后续报文的长度
			//字节存储，十进制方式
			if((ret>0) && strncasecmp(line,"Content-Length: ",16)==0)
			{
				content_len=atoi(&line[16]);  //拿到报文的长度
			}
		}while(ret>0 && strcmp(line,"\n"));
	
		if(content_len==-1)  
		{
			echo_string(sock);
			return 10;
		}
	}
	//已经读完报头信息，并且把空行和请求报头处理完

    const char *echo_line="HTTP/1.0 200 OK\r\n";
	send(sock, echo_line, strlen(echo_line), 0);
	const char *type="Content-Type:text/html;charset=ISO-8859-1\r\n";  //告诉浏览器正文是什么
	send(sock, type, strlen(type), 0);
	const char *null_line="\r\n";
	send(sock, null_line, strlen(null_line), 0);

	int input[2];   //读
	int output[2];  //写
	if(pipe(input)<0) 
	{
		echo_string(sock);
		return 11;
	}

	if(pipe(output)<0)
	{
		echo_string(sock);
		return 12;
	}

	//如果用execl，那么程序将被全部替换掉
	//execl只影响代码和数据，文件描述符是不会受到影响的
	//此时需要创建一个线程去执行需要替换的部分
	
	
	//一方面需要父进程区读取后续信息
	//另一方面，父进程需要向子进程传递参数
	//父进程将方法和参数写入管道中
	//环境变量是不会因为进程替换而改变的
	
	pid_t id=fork();
	if(id<0)
	{
		echo_string(sock);
		return 13;
	}
	else if(id==0)   //child
	{ 
		//execl(path,path,NULL);
		//exit(1);

		close(input[1]);  //关闭写端
		close(output[0]); //关闭读端

		char method_env[SIZE/8];
		char content_len_env[SIZE/8];
		char query_char_env[SIZE];

		memset(method_env,0,sizeof(method_env));
		memset(content_len_env,0,sizeof(content_len_env));
		memset(query_char_env,0,sizeof(query_char_env));
		
		sprintf(method_env,"METHOD=%s",method);
		putenv(method_env);

		if(strcasecmp(method,"GET")==0)
		{
			sprintf(query_char_env,"QUERY_STRING=%s",query_string);
			putenv(query_char_env);
		}
		else
		{
			sprintf(content_len_env,"CONTENT_LEN=%d",content_len);
			putenv(content_len_env);
			printf("content-len：%d\n",content_len);
		}

		dup2(input[0],0);   //输入重定向
		dup2(output[1],1);  //输出重定向
		execl(path,path,NULL); //第一个执行谁，第二个怎么执行
		exit(1);

	}//child
	else  //负责将数据输入到管道中，让子进程去执行
	{
		//int ret=waitpid(id,NULL,0);
		close(input[0]);
		close(output[1]);

		char ch='\0';
		if(strcasecmp(method,"POST")==0)  //读取消息正文部分
		{
			int i=0;
			for(;i<content_len;i++)
			{
				recv(sock,&ch,1,0);
				write(input[1],&ch,1);
			}
		}

		ch='\0';
		while(read(output[0],&ch,1)>0)  //文件描述符随进程，进程结束，将会读到0
		{
			send(sock,&ch,1,0);
		}
		waitpid(id,NULL,0);
		close(input[1]);
		close(output[0]);
	}//father
}

void *handler_request(void* arg)
{
	int sock=(int)arg;
#ifdef _DEBUG
	char line[1024];
	do{
		int ret=get_line(sock,line,sizeof(line));
		if(ret>0)
		{
			printf("%s",line);
		}
		else
		{
			printf("request done...\n");
			break;
		}
	}while(1);

#else

	int ret=0;
	char buf[SIZE];
	char method[SIZE/10];  //get method
	char url[SIZE];   //get url
	char path[SIZE]; //有效路径
	int i=0;  //method 
	int j=0;  //buf
	int cgi=0;
	char *query_string=NULL;

	if(get_line(sock,buf,sizeof(buf))<=0)
	{
		echo_string(sock);
		ret=5;
		goto end;
	}

	while(!isspace(buf[j]) && j<sizeof(buf) && i<sizeof(method)-1)   //获取处理方法
	{
		method[i]=buf[j];
		i++;
		j++;
	}

	method[i]='\0';
	//if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))  //判断method是否需要处理
	if(strcasecmp(method,"GET") && strcasecmp(method,"POST"))  //判断method是否需要处理
	{
		echo_string(sock);
		ret=6;
		goto end;
	}
	
	if(strcasecmp(method,"POST")==0)   //如果是POST，则需要进行cgi处理
	{
		cgi=1;
	}

	while(isspace(buf[j])&& j<sizeof(buf)) //"GET    /   http/1.0"
	{
		j++;
	}

	i=0;
	while(!isspace(buf[j])&& j<sizeof(buf) && i<sizeof(url)-1)  //获取路径url
	{
		url[i]=buf[j];
		i++;
		j++;
	}
	url[i]='\0';
	printf("method:%s,url:%s\n",method,url);


	query_string=url;
	while(*query_string!='\0')   //参数和路径分离，以‘？’分割
	{
		if(*query_string=='?')
		{
			*query_string='\0';
			query_string++;
			cgi=1;	
			printf("hehe\n");
			break;
		}
		query_string++;
	}
	
	printf("url---->%s\n",url);	
	sprintf(path,"wwwroot%s",url);
	
	if(path[strlen(path)-1]=='/') //请求目录为‘/’
	{
		//sprintf(path,"%sindex.html",url);  //path=/index.html
		strcat(path,"index.html");
	}
	printf("path----> %s\n",path);
	struct stat st;
	if(stat(path,&st)!=0)
	{
		echo_string(sock);
		printf("stat\n");
		ret=7;
		goto end;
	}
	else
	{
		if(S_ISDIR(st.st_mode))
		{
			strcat(path,"/index.html");
			printf("strcat\n");
		}
		else if((st.st_mode & S_IXUSR)||(st.st_mode & S_IXGRP)||(st.st_mode & S_IXOTH))
		{
			cgi=1;	
		}
		else
		{}

		if(cgi)
		{
			printf("enter cgi...\n");
			exe_cgi(sock,method,path,query_string);
		}
		else  //直接请求资源-->get
		{
		 	//printf("enter echo_www...\n");
			printf("wwwwwwmethod:%s,url:%s,path:%s,cgi:%d,query_string:%s\n",method,url,path,cgi,query_string);
			drop_header(sock);
			echo_www(sock,path,st.st_size);
		}
	}


end:
	close(sock);
	return (void*)ret;
#endif
}

