#ifndef __sql_agi__
#define __sql_agi__
#include<iostream>
#include<mysql.h>
#include<stdio.h>
#include<string.h>
class sqlAgi
{
	public:
		sqlAgi();
		int myConnect();
		int myInsert(const std::string& name,const std::string& sex,const std::string& age,\
				const std::string& hobby,const std:: string& school);
		int mySelect();
		~sqlAgi();

	private:
		MYSQL* conn_fd;
};

#endif

