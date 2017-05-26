#pragma once
#include"Lobby.h"

class RecvThreadClass
{
	HANDLE hSndThread, hRcvThread;
	static unsigned WINAPI RecvMsg(PVOID arg);
public:
	typedef struct
	{
		SOCKET sock;
		Lobby *lobby;

	}ThreadData;	ThreadData tData;
	RecvThreadClass();
	virtual ~RecvThreadClass();
	void StartThread();
	static bool ExitFlag, LogoutFlag;
	static bool threadOn;//�ӽ�
	static int ClientMode;//�α���ȭ��(0), �κ�(1), ����(2), ������(3);
};

