#pragma once
/*방 정보에 대한 것만*/
#include"grideXY.h"
#include<list>


class WaitingRoomInfo
{
private:
	/*WaitingRoom 기능 관리 함수, 변수*/
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

