#pragma once
#include "grideXY.h"
#include"UserInfo.h"
#include "GamePlayInfo.h"
class GamePlayClass :protected 	grideXY
{

	GamePlayInfo info;
	UserInfo *user;
	int key;
	int x, y;//player위치
	SOCKET sock;
public:
	GamePlayClass();
	~GamePlayClass();
	int mazeGameMain();
	void setUserInfo(UserInfo *input_user);
	bool SendInputKey(int key);
	void RecvPlayerPosition(char*);//유저키_방향키
private:
	void initPrint();
	void movePrint();
	void Ending();
	void EndingSubLoop(int a, char c);
	bool finalPoint();
	void move_arrow(int key);
};

