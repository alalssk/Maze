#pragma once

#include"LobbyInfo.h"
class Lobby :protected grideXY
{
	int key;//key �Է°���(����Ű ��)
	LobbyInfo Linfo;
	SOCKET sock;

	void PrintLobbyListBox();
	void AllClearPrintLobbyTxtBox();
	void PrintLobbyTxt();

	void initRoomListCheck();
public:
	Lobby();
	virtual ~Lobby();

	const int LobbyMain(SOCKET sock);//�ϴ� ��Ĺ�� ���ڷ� �޾Ƽ� send��û�� �ϵ��� ����
	void setSock(SOCKET sock);
	bool req_CreateRoom();
};

