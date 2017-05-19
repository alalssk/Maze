#pragma once
#include<my_global.h>
#include<mysql.h>

#include<iostream>
#include<string>

using namespace std;

#define DB_HOST "localhost"	// DB IP주소 또는 도메인 또는 localhost
#define DB_USER "root"		// DB접속 계정 명
#define DB_PASS "74498063"		// DB접속 계정 암호
#define DB_NAME "maze_server"		// DB 이름

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

