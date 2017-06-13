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
	static bool threadOn;//�ӽ�
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
	void setUserInfo(UserInfo*);
	
};

