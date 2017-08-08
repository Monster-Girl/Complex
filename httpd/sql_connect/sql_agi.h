#ifndef __sql_agi__
#define __sql_agi__
#include<iostream>
#include<mysql.h>
class sqlAgi
{
	public:
		sqlAgi();
		int myConnect();
		int myInsert();
		int mySelect();
		~sqlAgi();

	private:
		MYSQL* conn_fd;
};

#endif

