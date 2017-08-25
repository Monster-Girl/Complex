#include"sql_agi.h"
using namespace std;

sqlAgi::sqlAgi()
{
	conn_fd=mysql_init(NULL);
}

int sqlAgi::myConnect()
{
	if(mysql_real_connect(conn_fd,"127.0.0.1","root","123456","cal",3306,NULL,CLIENT_MULTI_RESULTS | CLIENT_MULTI_STATEMENTS))
		cout<<"connecct success"<< endl;
	else
		perror("mysql_real_connect");	
}

int sqlAgi::Find_Root(const string& name)
{
	string sql="select *from login where root='";
	sql+=name;
	sql+="'";
	if(mysql_query(conn_fd,sql.c_str())==0)
	{
		MYSQL_RES *res=mysql_store_result(conn_fd);
		if(!res)
		{
			cout<<"select root failed"<<endl;
		}
		else
		{
			int col=mysql_num_fields(res);
			int row=mysql_num_rows(res);
			MYSQL_ROW lines;
			int i=0;
			for(;i<row;i++)
			{
				lines=mysql_fetch_row(res);
				string tmp=lines[i];
				cout<<tmp<<endl;
				if(strcasecmp(tmp.c_str(),name.c_str())==0)
					return -1;
			}
			return 0;
		}
	}
}

int sqlAgi::Login(const string& name,const string& passwd)
{
	string sql="select *from login where root='";
	sql+=name;
	sql+="' and password='";
	sql+=passwd;
	sql+="';";
	cout<<sql<<endl;
	int ret=mysql_query(conn_fd,sql.c_str());
	if(ret==0)
		cout<<"login success"<<endl;
	else
		cout<<"name is not match password";
}

int sqlAgi::Insert_Login(const string& name,const string& passwd)
{
	int tmp=Find_Root(name);
	if(tmp!=0)
	{
		cout<<"root has login"<<endl;
		return 0;
	}

	string sql="insert into login(root,password) value('";
	sql+=name;
	sql+="','";
	sql+=passwd;
	sql+="')";

	cout<<sql<<endl;
	int ret=mysql_query(conn_fd,sql.c_str());
	cout << "fd:" << conn_fd << endl;
	if(ret!=0)
		cout<<"insert failed:"<< ret << endl;
	else 
		cout << "insert ok" << endl;
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
	string sql="select * from login";
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
