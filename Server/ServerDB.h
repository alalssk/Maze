#pragma once
#include<my_global.h>
#include<mysql.h>

#include<iostream>
#include<string>

using namespace std;

#define DB_HOST "localhost"	// DB IP�ּ� �Ǵ� ������ �Ǵ� localhost
#define DB_USER "root"		// DB���� ���� ��
#define DB_PASS "74498063"		// DB���� ���� ��ȣ
#define DB_NAME "Maze"		// DB �̸�

#define QUERY_FLAG  1



class ServerDB
{

public:
	typedef struct {
		int win_count;
		int Play_count;
	}UserCount, *LPUserCount;
	ServerDB();
	virtual ~ServerDB();
	bool StartDB();
	bool Insert_User(string id, string pass);
	bool Find_ID(string id);
	bool Check_Password(char* id_pass);
	bool OneIncreass_visit_count(string id);
	bool GetUserWinCount(string id, int &win, int &play);//_IN, _OUT, _OUT
private:
	MYSQL *connection = NULL, conn;



};

