#pragma once
#include"grideXY.h"
#define LOBBY_TXT_NUM_MAX 3
#define LOBBY_LIST_NUM_MAX 15
class LobbyInfo
{
	int LobbyTxtNum;
	int LobbyListNumber;
	bool LobbyFlag;

public:
	LobbyInfo();
	virtual ~LobbyInfo();

	/*lobby의 기능 출력을 위한 변수 세팅을 위한 함수들*/
	const int GetLobbyTxtNum();
	void SetLobbyTxtNum(const int);

	/*lobby의 기능(방만들기 로그아웃 종료)등의 기능과 방list선택 기능을 구분하는 플레그*/
	bool GetLobbyFlag();
	void SetLobbyFlag(int);


};

