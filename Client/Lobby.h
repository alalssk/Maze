#pragma once

#include"LobbyInfo.h"
#include"UserInfo.h"
#include<Windows.h>
#include<process.h>
#define BUF_SIZE 1024
class Lobby :protected grideXY
{
	int key;//key �Է°���(����Ű ��)
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

	void PrintWaitionRoomList();//��� ConnectToServer�� Recv������ �ȿ����� ȣ����
public:
	Lobby();
	virtual ~Lobby();
	void GetWaitionRoomList(char *);
	static HANDLE hEventForRequest;
	const int LobbyMain();
	void setUserInfo(UserInfo *user);



};

