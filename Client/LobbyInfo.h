#pragma once
#include"grideXY.h"
#define LOBBY_TXT_NUM_MAX 3
#define LOBBY_LIST_NUM_MAX 15
#define MAX_ROOM_NUM	9999
#define MAX_ROOM_NAME_SIZE	50
class LobbyInfo
{
private:
	int LobbyTxtNum;
	bool LobbyFlag;
	int LobbyListPointNum;
public:
	
	LobbyInfo();
	~LobbyInfo();

	int WaitingRoomListNum;//���� �� ����
	char WaitingRoomList[MAX_ROOM_NUM][MAX_ROOM_NAME_SIZE]; //[�ִ�氳��][�ִ� ������ ������]
	/*lobby�� ��� ����� ���� ���� ������ ���� �Լ���*/
	const int GetLobbyTxtNum();
	void SetLobbyTxtNum(const int);

	/*lobby�� ���(�游��� �α׾ƿ� ����)���� ��ɰ� ��list���� ����� �����ϴ� �÷���*/
	bool GetLobbyFlag();
	void SetLobbyFlag(int);

	void initLobbyListPointNumber();
	void SetLobbyListPointNumber(const int key);
	int GetLobbyListPointNumber();
};

