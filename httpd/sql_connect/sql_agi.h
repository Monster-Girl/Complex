#ifndef __sql_agi__
#define __sql_agi__
#include<iostream>
#include<mysql.h>
#include<stdio.h>
#include<string.h>
#include<string>
class sqlAgi
{
	public:
		sqlAgi();
		int myConnect();
		int Find_Root(const std::string& name);
		int Login(const std::string& name,const std::string& passwd);
		int Insert_Login(const std::string& root,const std::string& passwd);
		int myInsert(const std::string& company,const std::string& post,const std::string& online,const std::string& written,const std::string& delivery,const std::string& remarks);
		int mySelect(const std::string& table);
		~sqlAgi();

	private:
		MYSQL* conn_fd;
};

#endif

