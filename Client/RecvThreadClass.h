#pragma once
#include"Lobby.h"
#include"WaitingRoom.h"
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

	}ThreadData;	ThreadData tData;// RecvThread�� �Ķ��Ÿ�� �־��� ����Ÿ
	RecvThreadClass();
	virtual ~RecvThreadClass();
	void StartThread();
	static bool ExitFlag, LogoutFlag;
	static bool threadOn;//�ӽ�
	static int ClientMode;//�α���ȭ��(0), �κ�(1), ����(2), ������(3);

	void setUserInfo(UserInfo*);
	
};

