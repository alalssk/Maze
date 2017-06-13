#pragma once
#include<my_global.h>
#include<mysql.h>
#include<Windows.h>
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
	bool UserConnection(string id, string type); // NEW_ACOUNT, LOGIN, LOGOUT
	/*������� ���*/
	bool CreateWaitingRoom(string, int);//���̸�, ���ȣ
	bool DeleteWaitingRoom(int);
	bool JoinWaitingRoom(int, char *);//���ȣ �����̸�
	bool ExitWaitingRoom(int iRoomNum, char * UserName);//���ȣ �����̸�
	/*���Ӱ���*/
	bool StartPlayGame(int); //���ȣ insert >> game_tbl
	bool EndPlayGame(int gameNum);//���ӹ�ȣ update >> game_tbl
	int GetPlayingGameID(int roomNum); //���ȣ select
	bool GamePlayUserLog(int iRoomNum, char*UserName);//���ȣ, �����̸�(user_id)  insert >> play_game_tbl
	bool PlusWinCount(char *UserName);	//�����̸�, update >> user_tbl
	bool PlusPlayCount(char * UserName);//�����̸�, update >> user_tbl
	int GetTotalCreateRoomCount();
private:
	MYSQL *connection = NULL, conn;
	string GetStringTypeTime();
	bool ConnectionCheck();


};

