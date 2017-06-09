#include "LoginMainInfo.h"


LoginMainInfo::LoginMainInfo() :LoginArrNum(0), serverNum(0), inputIdSz(0), inputPassSz(0)
{
	memset(userID, 0, sizeof(userID));
	memset(userPass, 0, sizeof(userPass));
}

LoginMainInfo::~LoginMainInfo()
{

	inputPassSz = 0;
	inputIdSz = 0;
	LoginArrNum = 0;
	serverNum = 0;
	memset(userID, 0, sizeof(userID));
	memset(userPass, 0, sizeof(userPass));
}
/*login & serve선택 변수 관련*/
void LoginMainInfo::setLoginArrPlus()
{
	if (++LoginArrNum == 5) LoginArrNum = 0;
}
void LoginMainInfo::setLoginArrMinus()
{
	if (--LoginArrNum == -1) LoginArrNum = 4;
}
int LoginMainInfo::getLoginArrNum()
{
	//	cout << LoginArrNum << endl;
	return this->LoginArrNum;
}

void LoginMainInfo::setServerPlus()
{
	if (++serverNum == 3) serverNum = 0;
}
void LoginMainInfo::setServerMinus()
{
	if (--serverNum == -1) serverNum = 2;
}
int LoginMainInfo::getServerNum()
{
	//	cout << serverNum << endl;
	return this->serverNum;
}


int LoginMainInfo::cInputUserID(char key)
{
	if (key >= 48 && key <= 122)//알파뱃과 일부 분자만 가능하게 바꾸자
	{
		userID[getUserIdSz()] = key;
		inputIdSz++;
	}
	else if (key == BACK_SPACE)//backspace
	{
		inputIdSz--;
		memset(userID + getUserIdSz(), 0, sizeof(char));
	}

	return getUserIdSz();
}
int LoginMainInfo::getUserIdSz()
{
	return this->inputIdSz;
}
char* LoginMainInfo::getUserId()
{
	return this->userID;
}

int LoginMainInfo::cInputUserPass(char key)
{
	if (key >= 48 && key <= 122)//알파뱃과 일부 분자만 가능하게 바꾸자
	{
		userPass[getUserPassSz()] = key;
		inputPassSz++;
	}
	else if (key == BACK_SPACE)//backspace
	{
		inputPassSz--;
		memset(userPass + getUserPassSz(), 0, sizeof(char));
	}

	return getUserPassSz();
}
int LoginMainInfo::getUserPassSz()
{
	return this->inputPassSz;
}
char* LoginMainInfo::getUserPass()
{
	return this->userPass;
}
void LoginMainInfo::initLoginArr()
{
	LoginArrNum = 0;
}
void LoginMainInfo::initID_Pass()
{
	inputPassSz = 0;
	inputIdSz = 0;
	memset(userID, 0, sizeof(userID));
	memset(userPass, 0, sizeof(userPass));
}