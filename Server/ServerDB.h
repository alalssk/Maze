#pragma once
#include<my_global.h>
#include<mysql.h>

#include<iostream>
#include<string>

using namespace std;

#define DB_HOST "localhost"	// DB IP�ּ� �Ǵ� ������ �Ǵ� localhost
#define DB_USER "root"		// DB���� ���� ��
#define DB_PASS "74498063"		// DB���� ���� ��ȣ
#define DB_NAME "maze_server"		// DB �̸�

#define QUERY_FLAG  1



class ServerDB
{
public:
	ServerDB();
	virtual ~ServerDB();
	void StartDB();
private:
	MYSQL *connection = NULL, conn;
	MYSQL_RES * sql_result;
	MYSQL_ROW sql_row;
	int query_stat;
};

