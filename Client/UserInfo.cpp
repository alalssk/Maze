#include "UserInfo.h"



UserInfo::UserInfo() :RoomState(false)
{
	memset(userID_Password_for_send, 0, sizeof(userID_Password_for_send));
}


UserInfo::~UserInfo()
{
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
	ChangeRoomState();
}
bool UserInfo::ChangeRoomState()
{
	if (RoomState) RoomState = false;
	else RoomState = true;
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

	ChangeRoomState();

}