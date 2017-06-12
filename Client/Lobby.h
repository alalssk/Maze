#pragma once

#include"LobbyInfo.h"
#include"UserInfo.h"
#include<Windows.h>
#include<process.h>
#define BUF_SIZE 1024
class Lobby :protected grideXY
{
private:
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
	bool req_EnterWaitingRoom(int RoomNum);
	//bool req_EnterWaitingRoom(char* );
	void setSock(SOCKET sock);

	void PrintWaitingRoomList();//��� ConnectToServer�� Recv������ �ȿ����� ȣ����
public:
	Lobby();
	virtual ~Lobby();
	void GetWaitingRoomList(char *);
	static HANDLE hLobbyEventForRequest;
	const int LobbyMain();
	void setUserInfo(UserInfo *user);



};

