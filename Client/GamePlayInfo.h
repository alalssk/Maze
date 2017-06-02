#pragma once
#include<iostream>
#include<string>
#define X_SIZE 63
#define Y_SIZE 32
using namespace std;
class GamePlayInfo
{
	char One[15][11 * 2];
	char Two[15][11 * 2];
	char Three[15][11 * 2];
public:

	char **gameMap;
	GamePlayInfo();
	~GamePlayInfo();
	void ReadMap();
	void grideMap();
	void PrintOne();
	void PrintTwo();
	void PrintThree();
};
