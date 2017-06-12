#pragma once
#include"LoginMain.h"
#include"Lobby.h"
#include"RecvThreadClass.h"
#include"WaitingRoom.h"
#include"GamePlayClass.h"
#include"GamePlayInfo.h"
#define CONNECTION_CODE 1
#define CREATE_ROOM 0
#define JOIN_ROOM 3
#define LOGOUT_CODE 4
#define EXIT_CODE 2
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

