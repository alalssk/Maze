#pragma once
/*�� ������ ���� �͸�*/
#include"grideXY.h"
#include<list>


class WaitingRoomInfo
{
	typedef struct
	{
		int RoomNumber;
		char RoomName[40];
		char OwnerID[20];
		//userInfo class �� ����� ���� ���⿡d
		//userInfo users[3];
		//int userCount; (max�� 3)
		//�̴� ��Ƽ����� ������ �� �ʿ���.
	}RoomInfo;
	list <RoomInfo> Roomlist;//�̰� Ŭ�󿡼� �ʿ��Ѱ�...? �κ񿡼� �� ����Ʈ ������ַ��� ��¿������...?
	list<string> chatMemory; // ��¿� ä�÷α� ����Ʈ
	/*WaitingRoom ��� ���� �Լ�, ����*/

	bool WaitingRoomFlag;
	int WaitingRoomTxtNum;
	int gitgit;//githubȮ�ο�
public:
	const bool GetWaitingRoomFlag();
	void SetWaitingRoomFlag(const int key);// chat or choose(start, exit)
	const int GetWaitingRoomTxtNum();
	void SetWaitingRoomTxtNum(const int key); //start or exit
	//


public:
	WaitingRoomInfo();
	~WaitingRoomInfo();

	bool RoomsInfoFromServer();
	list<RoomInfo> getRoomInfo();


};

