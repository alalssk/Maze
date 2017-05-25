#pragma once

#include"LobbyInfo.h"
#include<Windows.h>
#include<process.h>
#define BUF_SIZE 1024
class Lobby :protected grideXY
{
	int key;//key 입력관련(방향키 등)
	LobbyInfo Linfo;
	SOCKET sock;

	/*스레드전용*/
	typedef struct
	{
		SOCKET sock;
		Lobby *lobby;
	
	}ThreadData;
	ThreadData td;
	HANDLE hSndThread, hRcvThread;
	static HANDLE hEventForRequest;
	static bool threadOn;//임시
	static unsigned WINAPI RecvMsg(PVOID arg);

	static int ClientMode;//로그인화면(0), 로비(1), 대기방(2), 게임중(3);
	/*스레드전용*/

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
	void PrintWaitionRoomList();//얘는 ConnectToServer의 Recv쓰레드 안에서만 호출함
public:
	Lobby();
	virtual ~Lobby();
	static bool ExitFlag, LogoutFlag;
	const int LobbyMain(SOCKET sock);//일단 소캣을 인자로 받아서 send요청을 하도록 하자
	void setSock(SOCKET sock);


};

