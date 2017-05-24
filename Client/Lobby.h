#pragma once

#include"LobbyInfo.h"
class Lobby :protected grideXY
{
	int key;//key 입력관련(방향키 등)
	LobbyInfo Linfo;
	SOCKET sock;

	void PrintLobbyListBox();
	void AllClearPrintLobbyTxtBox();
	void PrintLobbyTxt();

	void initRoomListCheck();
	
	void initPrintWaitingRoomList();
	void PrintLobbyListCheck(int);
public:
	Lobby();
	virtual ~Lobby();

	const int LobbyMain(SOCKET sock);//일단 소캣을 인자로 받아서 send요청을 하도록 하자
	void setSock(SOCKET sock);
	bool req_CreateRoom();

	void PrintWaitionRoomList(char *);//얘는 ConnectToServer의 Recv쓰레드 안에서만 호출함
};

