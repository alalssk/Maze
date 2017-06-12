#pragma once
#include"LoginMain.h"
#include"Lobby.h"
#include"RecvThreadClass.h"
#include"WaitingRoom.h"
#include"GamePlayClass.h"
#include"GamePlayInfo.h"

class GameManager
{
private:
	LoginMain lmain;
	Lobby Lobby;
	RecvThreadClass rThre;
	WaitingRoom wRoom;
	GamePlayClass gPlay;
	UserInfo user;
	int code;

public:
	GameManager();
	~GameManager();
	void initGame();
	bool RunGame();
};

