#pragma once
#include"Lobby.h"
#include"WaitingRoom.h"
#include"GamePlayClass.h"
class RecvThreadClass
{
	HANDLE hSndThread, hRcvThread;
	static unsigned WINAPI RecvMsg(PVOID arg);
	
public:

	typedef struct
	{
		SOCKET sock;
		UserInfo *user;
		Lobby *lobby;
		WaitingRoom *wRoom;
		GamePlayClass *gPlay;

	}ThreadData;	ThreadData tData;// RecvThread의 파라메타로 넣어줄 데이타
	RecvThreadClass();
	virtual ~RecvThreadClass();
	void StartThread();
	static bool ExitFlag, LogoutFlag;
	static bool threadOn;//임시
	static int ClientMode;//로그인화면(0), 로비(1), 대기방(2), 게임중(3);

	void setUserInfo(UserInfo*);
	
};

