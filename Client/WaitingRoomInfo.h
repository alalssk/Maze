#pragma once
/*�� ������ ���� �͸�*/
#include"grideXY.h"
#include<list>


class WaitingRoomInfo
{
private:
	/*WaitingRoom ��� ���� �Լ�, ����*/
	bool WaitingRoomFlag;
	int WaitingRoomTxtNum;

public:
	WaitingRoomInfo();
	~WaitingRoomInfo();

	const bool GetWaitingRoomFlag();
	void SetWaitingRoomFlag(const int key);// chat or choose(start, exit)
	const int GetWaitingRoomTxtNum();
	void SetWaitingRoomTxtNum(const int key); //start or exit
	void initWaitingRoomInfo();

};

