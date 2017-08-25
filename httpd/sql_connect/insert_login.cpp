/*************************************************************************
	> File Name: insert_login.cpp
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Tue 22 Aug 2017 05:18:30 PM CST
 ************************************************************************/

#include"sql_agi.h"
using namespace std;
int main()
{
	sqlAgi mydb;
	mydb.myConnect();
	mydb.Insert_Login("hello","111111");
	mydb.mySelect();
	return 0;
}
