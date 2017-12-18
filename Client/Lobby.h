#pragma once

#include"LobbyInfo.h"
#include"UserInfo.h"
#include<Windows.h>
#include<process.h>
#define BUF_SIZE 1024

#define CONNECTION_CODE 1
#define CREATE_ROOM 0
#define JOIN_ROOM 3
#define LOGOUT_CODE 4
#define EXIT_CODE 2
class Lobby
{
private:
	int key;//key 입력관련(방향키 등)
	SOCKET sock;
	UserInfo *user;
	LobbyInfo Linfo;
	grideXY gride;
	void PrintLobbyListBox();
	void AllClearPrintLobbyTxtBox();
	void PrintLobbyTxt();

	void initRoomListCheck();
	
	void initPrintWaitingRoomList();
	void PrintLobbyListCheck(int);
	bool req_CreateRoom();
	bool req_ExitClient();
	bool req_LogoutClient();
	bool req_GetWaitingRoom();
	bool req_EnterWaitingRoom(int RoomNum);
	//bool req_EnterWaitingRoom(char* );
	void setSock(SOCKET sock);

	void PrintWaitingRoomList();//얘는 ConnectToServer의 Recv쓰레드 안에서만 호출함
public:
	Lobby();
	virtual ~Lobby();
	static HANDLE hLobbyEventForRequest;

	void GetWaitingRoomList(char *);
	const int LobbyMain();
	void setUserInfo(UserInfo *user);



};

