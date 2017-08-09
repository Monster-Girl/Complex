#include"sql_agi.h"
using namespace std;

sqlAgi::sqlAgi()
{
	conn_fd=mysql_init(NULL);
}

int sqlAgi::myConnect()
{
	if(mysql_real_connect(conn_fd,"127.0.0.1","root","123456","info",3306,NULL,0))
		cout<<"connecct success"<<endl;
	else
	//	cout<<"connecct failed"<<endl;
	perror("mysql_real_connect");
}

int sqlAgi::myInsert(const string& name,const string& sex,const string& age,\
		const string& hobby,const string& school)
{
	string sql="insert into st_info(name,sex,age,hobby,school) value('";
	sql+=name;
	sql+="','";
	sql+=sex;
	sql+="','";
	sql+=age;
	sql+="','";
	sql+=hobby;
	sql+="','";
	sql+=school;
	sql+="')";

	cout<<sql<<endl;
	int ret=mysql_query(conn_fd,sql.c_str());
	if(ret!=0)
		cout<<"insert failed"<<endl;
}

int sqlAgi::mySelect()
{
	string sql="select * from st_info";
	if(mysql_query(conn_fd,sql.c_str())==0)
	{
		MYSQL_RES *res=mysql_store_result(conn_fd);
		if(!res)
		{
			cout<<"select failed!\n"<<endl;
		}
		else
		{
			int nums=mysql_num_rows(res);
			int col=mysql_num_fields(res);
			MYSQL_FIELD *fd;
			for(;fd=mysql_fetch_field(res);)
			{
				cout.setf(ios::left);
				cout.width(9);
				cout<<fd->name<<" ";
			}
			cout<<endl;


			int i=0;
			MYSQL_ROW lines;
			for(i=0;i<nums;i++)
			{
				lines=mysql_fetch_row(res);
				int j=0;
				for(;j<col;j++)
				{
					cout.setf(ios::left);
					cout.width(9);
					cout<<lines[j]<<" ";
				}
				cout<<endl;
			}

		}
	}
}

sqlAgi::~sqlAgi()
{
	mysql_close(conn_fd);
}
