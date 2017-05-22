#pragma once

#include"LobbyInfo.h"
class Lobby :protected grideXY
{
	int key;//key 입력관련(방향키 등)
	LobbyInfo Linfo;


	void PrintLobbyListBox();
	void AllClearPrintLobbyTxtBox();
	void PrintLobbyTxt();

	void initRoomListCheck();
public:
	Lobby();
	virtual ~Lobby();

	const int LobbyMain();
};

