/*************************************************************************
	> File Name: insert_info.cpp
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Sun 27 Aug 2017 04:22:27 PM CST
 ************************************************************************/

#include"sql_agi.h"
void insert(const char *string_arg)
{
		char arg_buf[1024];
		char *argv_buf[7];
		strcpy(arg_buf,string_arg);
		char *start=arg_buf;
		int i=0;
		while(*start)
		{
			if(*start=='=')
			{
				start++;
				argv_buf[i++]=start;
				continue;
			}
			if(*start=='&')
				*start=0;
			start++;
		}
		argv_buf[i]=0;
		sqlAgi mydb;
		mydb.myConnect();
		mydb.myInsert(argv_buf[0],argv_buf[1],argv_buf[2],argv_buf[3],argv_buf[4],argv_buf[5]);
}
											
int main()
{
	char *method=NULL;
	char *query_string=NULL;
	char *string_arg=NULL;
	int content_len=-1;
	char buf[1024];
	
	if((method=getenv("METHOD")))
	{
		if(strcasecmp(method,"GET")==0)	
		{
			if((query_string=getenv("QUERY_STRING")))
			{
				string_arg=query_string;
			}
		}	
		else
		{
			if(getenv("CONTENT_LEN"))
			{												
				content_len=atoi(getenv("CONTENT_LEN"));
				int i=0;
				for(;i<content_len;i++)
					read(0,&buf[i],1);
				buf[i]=0;
				string_arg=buf;																			}
		}
	}
	insert(string_arg);	

	//sqlAgi mydb;
	//mydb.myConnect();
	//mydb.myInsert("搜狐","技术类","2017-8-22","2017-08-28 19：00：00","是","校招");
   //mydb.mySelect("hr_info");
	return 0;
}
