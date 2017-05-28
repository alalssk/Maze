#pragma once

#include"LobbyInfo.h"
#include"UserInfo.h"
#include<Windows.h>
#include<process.h>
#define BUF_SIZE 1024
class Lobby :protected grideXY
{
	int key;//key 입력관련(방향키 등)
	LobbyInfo Linfo;
	SOCKET sock;


	UserInfo *user;

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
	void setSock(SOCKET sock);

	void PrintWaitionRoomList();//얘는 ConnectToServer의 Recv쓰레드 안에서만 호출함
public:
	Lobby();
	virtual ~Lobby();
	void GetWaitionRoomList(char *);
	static HANDLE hEventForRequest;
	const int LobbyMain();
	void setUserInfo(UserInfo *user);



};

