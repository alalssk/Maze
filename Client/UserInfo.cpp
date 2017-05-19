#include "UserInfo.h"



UserInfo::UserInfo()
{
	memset(userID_Password_for_send, 0, sizeof(userID_Password_for_send));
}


UserInfo::~UserInfo()
{
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