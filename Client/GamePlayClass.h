#pragma once
#include "grideXY.h"
#include"UserInfo.h"
#include "GamePlayInfo.h"
class GamePlayClass :protected 	grideXY
{

	GamePlayInfo info;
	UserInfo *user;
	int key;
	int x, y;//playerÀ§Ä¡
public:
	GamePlayClass();
	~GamePlayClass();
	int mazeGameMain();
	void setUserInfo(UserInfo *input_user);
private:
	void initPrint();
	void movePrint();
	void Ending();
	void EndingSubLoop(int a, char c);
	bool finalPoint();
	void move_arrow(int key);
};

