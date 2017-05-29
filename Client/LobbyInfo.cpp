#include "LobbyInfo.h"


LobbyInfo::LobbyInfo() :LobbyTxtNum(0), LobbyListPointNum(0), LobbyFlag(false), WaitingRoomListNum(0)
{
}


LobbyInfo::~LobbyInfo()
{
	LobbyTxtNum=0;
	LobbyFlag=0;
	LobbyListPointNum = 0;

}

const int LobbyInfo::GetLobbyTxtNum()
{
	return this->LobbyTxtNum;
}
void  LobbyInfo::SetLobbyTxtNum(const int key)
{
	if (key == UP)
	{
		if (LobbyTxtNum > 0) LobbyTxtNum--;
	}
	else if (key == DOWN)
	{
		if (LobbyTxtNum < LOBBY_TXT_NUM_MAX - 1)LobbyTxtNum++;
	}
}

bool LobbyInfo::GetLobbyFlag()
{
	return this->LobbyFlag;
}
void LobbyInfo::SetLobbyFlag(const int key)
{
	if (key == LEFT || key == RIGHT)
	{
		if (LobbyFlag == false)this->LobbyFlag = true;
		else this->LobbyFlag = false;
	}
}
void LobbyInfo::SetLobbyListPointNumber(const int key)
{
	if (key == UP)
	{
		if (LobbyListPointNum > 0) LobbyListPointNum--;
	}
	else if (key == DOWN)
	{
		if (LobbyListPointNum < WaitingRoomListNum-1) LobbyListPointNum++;//현재 방 개수만큼만 올릴수있음
	}
}
int LobbyInfo::GetLobbyListPointNumber()
{
	return this->LobbyListPointNum;
}
void LobbyInfo::initLobbyListPointNumber()
{
	LobbyListPointNum = 0;
}