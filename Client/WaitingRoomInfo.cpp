#include "WaitingRoomInfo.h"


WaitingRoomInfo::WaitingRoomInfo() :WaitingRoomFlag(false), WaitingRoomTxtNum(1)
{
}


WaitingRoomInfo::~WaitingRoomInfo()
{
	Roomlist.clear();
}
const bool WaitingRoomInfo::GetWaitingRoomFlag()
{
	return this->WaitingRoomFlag;
}
void WaitingRoomInfo::SetWaitingRoomFlag(const int key)
{
	if (key == LEFT || key == RIGHT)
	{
		if (WaitingRoomFlag == false)this->WaitingRoomFlag = true;
		else this->WaitingRoomFlag = false;
	}
}
const int WaitingRoomInfo::GetWaitingRoomTxtNum()
{
	return this->WaitingRoomTxtNum;
}
void  WaitingRoomInfo::SetWaitingRoomTxtNum(const int key)//start or exit
{
	if (key == UP)
	{
		if (WaitingRoomTxtNum > 1) WaitingRoomTxtNum--;
	}
	else if (key == DOWN)
	{
		if (WaitingRoomTxtNum < 2)WaitingRoomTxtNum++;
	}
}
