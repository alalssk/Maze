#pragma once
#include"grideXY.h"
#define LOBBY_TXT_NUM_MAX 3
#define LOBBY_LIST_NUM_MAX 15
class LobbyInfo
{
	int LobbyTxtNum;

	bool LobbyFlag;
	int LobbyListPointNum;


public:
	
	LobbyInfo();
	virtual ~LobbyInfo();

	int WaitingRoomListNum;//���� �� ����
	char WaitingRoomList[9999][50];
	/*lobby�� ��� ����� ���� ���� ������ ���� �Լ���*/
	const int GetLobbyTxtNum();
	void SetLobbyTxtNum(const int);

	/*lobby�� ���(�游��� �α׾ƿ� ����)���� ��ɰ� ��list���� ����� �����ϴ� �÷���*/
	bool GetLobbyFlag();
	void SetLobbyFlag(int);
	void SetLobbyListPointNumber(const int key);
	int GetLobbyListPointNumber();
};

