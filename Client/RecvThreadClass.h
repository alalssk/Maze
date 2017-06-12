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

	}ThreadData;	ThreadData tData;// RecvThread�� �Ķ��Ÿ�� �־��� ����Ÿ
	RecvThreadClass();
	virtual ~RecvThreadClass();
	void StartThread();
	static bool ExitFlag, LogoutFlag;
	static bool threadOn;//�ӽ�

	void setUserInfo(UserInfo*);
	
};

