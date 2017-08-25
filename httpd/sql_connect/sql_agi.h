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
		int myInsert(const std::string& name,const std::string& sex,const std::string& age,\
				const std::string& hobby,const std:: string& school);
		int mySelect();
		~sqlAgi();

	private:
		MYSQL* conn_fd;
};

#endif

