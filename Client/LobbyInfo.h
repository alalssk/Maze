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

	/*lobby�� ��� ����� ���� ���� ������ ���� �Լ���*/
	const int GetLobbyTxtNum();
	void SetLobbyTxtNum(const int);

	/*lobby�� ���(�游��� �α׾ƿ� ����)���� ��ɰ� ��list���� ����� �����ϴ� �÷���*/
	bool GetLobbyFlag();
	void SetLobbyFlag(int);


};

