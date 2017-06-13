#pragma once
#include"Lobby.h"
#include"WaitingRoom.h"
#include"GamePlayClass.h"
class RecvThreadClass
{
private:

	HANDLE hRcvThread;
	static unsigned WINAPI RecvMsg(PVOID arg);
	
public:
	static bool ExitFlag, LogoutFlag;
	static bool threadOn;//임시
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
	void setUserInfo(UserInfo*);
	
};

