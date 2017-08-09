#include"sql_agi.h"

void insert(const char *string_arg)
{
	char arg_buf[1024];
	char *argv_buf[6];
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
	mydb.myInsert(argv_buf[0],argv_buf[1],argv_buf[2],argv_buf[3],argv_buf[4]);
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
				string_arg=buf;
			}
		}
	}

	insert(string_arg);
	
	//测试
	//sqlAgi mydb;
	//mydb.myConnect();
	//mydb.myInsert("peter","man","18","coding","sust");
	return 0;
}
