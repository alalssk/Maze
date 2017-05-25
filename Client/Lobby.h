#pragma once

#include"LobbyInfo.h"
#include<Windows.h>
#include<process.h>
#define BUF_SIZE 1024
class Lobby :protected grideXY
{
	int key;//key �Է°���(����Ű ��)
	LobbyInfo Linfo;
	SOCKET sock;

	/*����������*/
	typedef struct
	{
		SOCKET sock;
		Lobby *lobby;
	
	}ThreadData;
	ThreadData td;
	HANDLE hSndThread, hRcvThread;
	static HANDLE hEventForRequest;
	static bool threadOn;//�ӽ�
	static unsigned WINAPI RecvMsg(PVOID arg);

	static int ClientMode;//�α���ȭ��(0), �κ�(1), ����(2), ������(3);
	/*����������*/

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

	void GetWaitionRoomList(char *);
	void PrintWaitionRoomList();//��� ConnectToServer�� Recv������ �ȿ����� ȣ����
public:
	Lobby();
	virtual ~Lobby();
	static bool ExitFlag, LogoutFlag;
	const int LobbyMain(SOCKET sock);//�ϴ� ��Ĺ�� ���ڷ� �޾Ƽ� send��û�� �ϵ��� ����
	void setSock(SOCKET sock);


};

