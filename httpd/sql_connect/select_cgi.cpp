#include"sql_agi.h"

void select(const char *string_arg)
{
	sqlAgi mydb;
	mydb.myConnect();
	mydb.mySelect("hr_info");
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
				string_arg=query_string;
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

	select(string_arg);
	//sqlAgi mydb;
	//mydb.myConnect();
	//mydb.mySelect();
	return 0;
}
