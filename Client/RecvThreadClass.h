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
	static bool threadOn;//임시
	static int ClientMode;//로그인화면(0), 로비(1), 대기방(2), 게임중(3);
};

