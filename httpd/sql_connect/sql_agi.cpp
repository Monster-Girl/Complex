#include"sql_agi.h"

using namespace std;

sqlAgi::sqlAgi()
{
	conn_fd=mysql_init(NULL);
}

int sqlAgi::myConnect()
{
	if(mysql_real_connect(conn_fd,"127.0.0.1","root","st_info","3306",NULL,0))
		cout<<"connecct success"<<endl;
	else
		cout<<"connecct failed"<<endl;
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



sqlAgi::~sqlAgi()
{
	mysql_close(conn_fd);
}
