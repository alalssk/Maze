#include "LobbyInfo.h"


LobbyInfo::LobbyInfo() :LobbyTxtNum(0), LobbyListNumber(0), LobbyFlag(false)
{
}


LobbyInfo::~LobbyInfo()
{
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