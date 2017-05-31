#include "UserInfo.h"



UserInfo::UserInfo() :RoomState(false)
{

}


UserInfo::~UserInfo()
{
}
void UserInfo::initUserInfoData()
{
	memset(id, 0, sizeof(id));
	memset(password, 0, sizeof(password));
	memset(userID_Password_for_send, 0, sizeof(userID_Password_for_send));
	WinCount = 0;
	sock = NULL;
	this->state = 0;
	wData.ConnectUserNum = 0;
	memset(wData.RoomName, 0, sizeof(wData.RoomName));
	memset(wData.UserName, 0, sizeof(wData.UserName));
	wData.RoomNum = 0;
	for (int i = 0; i < 3; i++)
	{
		wData.state[i] = false;
		wData.winCount[i] = 0;
	}
}
UserInfo UserInfo::getUserInfoClass()
{
	return *this;
}
bool UserInfo::setID(char *id)
{
	if (strlen(id) > USER_ID_SIZE)
	{
		return false;//input id가 13보다 클때
	}
	else
	{
		strcpy(this->id, id);
		return true;
	}
}
char* UserInfo::getID()
{
	return id;
}
bool UserInfo::setPassword(char *pass)
{
	if (strlen(pass) > USER_PASS_SIZE)
	{
		return false;
	}
	else
	{
		strcpy(this->password, pass);
		return true;
	}
}
char* UserInfo::getPassword()
{
	return password;
}
char* UserInfo::conv_ID_Password()
{
	strcat(userID_Password_for_send, id);
	strcat(userID_Password_for_send, "_");
	strcat(userID_Password_for_send, password);

	return userID_Password_for_send;
}
void UserInfo::initUserInfo()
{
	memset(id, 0, sizeof(id));
	memset(password, 0, sizeof(password));
	memset(userID_Password_for_send, 0, sizeof(userID_Password_for_send));
}
void UserInfo::setSocket(SOCKET sock)
{
	this->sock = sock;
}
SOCKET UserInfo::getSocket()
{
	return sock;
}
void UserInfo::setWaitingRoomData(char* input)//ChangeRoomState();
{//방번호_방제목
	int iRoomNum;
	char *tmp;
	tmp = strtok(input, "_");
	iRoomNum = atoi(tmp);

	wData.RoomNum = iRoomNum;
	tmp = strtok(NULL, "_");
	strcpy(wData.RoomName, tmp);
	if (wData.ConnectUserNum == 0)
	{
		strcpy(wData.UserName[0], id);
		wData.state[0] = true;
		wData.winCount[0] = 10;//임시
		wData.ConnectUserNum = 1;
	}
	setRoomState(true);
}
void UserInfo::setWaitingRoomUserList(char* input)
{//alalssk-5_test-10_
	char *tmp;
	int iUserNum = 0;
	memset(wData.UserName, 0, sizeof(wData.UserName));
	tmp = strtok(input, "_");
	while (tmp)
	{
		strcpy(wData.UserName[iUserNum++], tmp);
		tmp = strtok(NULL, "_");
	}
	wData.ConnectUserNum = iUserNum;
}
bool UserInfo::setRoomState(bool state)
{
	this->RoomState = state;
	return RoomState;
}
bool UserInfo::getRoomState()
{
	return this->RoomState;
}

void UserInfo::ExitWaitingRoom()
{
	wData.RoomNum = 0;
	memset(wData.RoomName, 0, sizeof(wData.RoomName));
	memset(wData.state, 0, sizeof(wData.state));
	memset(wData.UserName, 0, sizeof(wData.UserName));
	memset(wData.winCount, 0, sizeof(wData.winCount));
	wData.ConnectUserNum = 0;

	setRoomState(false);

}