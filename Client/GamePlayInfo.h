#pragma once
#include<iostream>
#include<string>
#define X_SIZE 63
#define Y_SIZE 32
using namespace std;
class GamePlayInfo
{
public:
	char **gameMap;
	GamePlayInfo();
	~GamePlayInfo();
	void ReadMap();
	void grideMap();
};
