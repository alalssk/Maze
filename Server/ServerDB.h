#pragma once
#include<my_global.h>
#include<mysql.h>
#include<Windows.h>
#include<iostream>
#include<string>

using namespace std;

#define DB_HOST "localhost"	// DB IP주소 또는 도메인 또는 localhost
#define DB_USER "root"		// DB접속 계정 명
#define DB_PASS "74498063"		// DB접속 계정 암호
#define DB_NAME "Maze"		// DB 이름

#define QUERY_FLAG  1



class ServerDB
{
	SYSTEMTIME sysTime;
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
	/*방관련 디비*/
	bool CreateWaitingRoom(string, int);//방이름, 방번호
	bool DeleteWaitingRoom(string, int);
	bool StartPlayGame(int); //방번호
	bool EndPlayGame(int);
	int GetTotalCreateRoomCount();

private:
	MYSQL *connection = NULL, conn;



};

