#pragma once

#include"LobbyInfo.h"
class Lobby :protected grideXY
{
	int key;//key �Է°���(����Ű ��)
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

