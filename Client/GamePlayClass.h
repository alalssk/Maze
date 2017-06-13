#pragma once
#include "grideXY.h"
#include"UserInfo.h"
#include "GamePlayInfo.h"
class GamePlayClass
{
private:
	GamePlayInfo info;
	UserInfo *user;
	int MyKey;
	grideXY gride;
	SOCKET sock;
public:
	GamePlayClass();
	~GamePlayClass();
	int mazeGameMain();
	void setUserInfo(UserInfo *input_user);
	bool SendInputKey(int key);
	void RecvPlayerPosition(char* );//����Ű_����Ű
private:
	void initPrint();
	void movePrint();
	void Ending();
	void EndingSubLoop(int a, char c);
	bool finalPoint();
	int GetRandomKey();
};

