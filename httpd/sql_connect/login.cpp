/*************************************************************************
	> File Name: login.cpp
	> Author: monster
	> Mail: 1104306242@qq.com 
	> Created Time: Tue 22 Aug 2017 05:01:02 PM CST
 ************************************************************************/

#include"sql_agi.h"

using namespace std;

int main()
{
	sqlAgi mydb;
	mydb.myConnect();
	mydb.Login("peter","123456");
	return 0;
}
