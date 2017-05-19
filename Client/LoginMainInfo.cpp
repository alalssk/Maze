#include "LoginMainInfo.h"


LoginMainInfo::LoginMainInfo() :LoginArrNum(0), serverNum(0), inputIdSz(0), inputPassSz(0)
{
	memset(userID, 0, sizeof(userID));
	memset(userPass, 0, sizeof(userPass));
}

LoginMainInfo::~LoginMainInfo()
{
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

/*ID Password 관련*/
//void LoginMainInfo::setUserID(char* userID)
//{
//	if (strlen(this->userID) > strlen(userID)) //백스페이스가 눌린 경우임!
//	{
//		memset(this->userID, 0, sizeof(this->userID));
//	}
//	strcpy(this->userID, userID);
//	setUserIdSz(strlen(this->userID));
//
//}
//void LoginMainInfo::setUserIdSz(int size)
//{
//	this->inputIdSz = size;
//}
int LoginMainInfo::cInputUserID(char key)
{
	if (key >= 48 && key <= 122)//알파뱃과 일부 분자만 가능하게 바꾸자
	{
		userID[getUserIdSz()] = key;
		inputIdSz++;
	}
	else if (key == 8)//backspace
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

//void LoginMainInfo::setUserPass(char* userPass)
//{
//	if (strlen(this->userPass) > strlen(userPass))
//	{
//		memset(this->userPass, 0, sizeof(this->userPass));
//	}
//	strcpy(this->userPass, userPass);
//	setUserPassSz(strlen(this->userPass));
//}
//void LoginMainInfo::setUserPassSz(int size)
//{
//	this->inputPassSz = size;
//}
int LoginMainInfo::cInputUserPass(char key)
{
	if (key >= 48 && key <= 122)//알파뱃과 일부 분자만 가능하게 바꾸자
	{
		userPass[getUserPassSz()] = key;
		inputPassSz++;
	}
	else if (key == 8)//backspace
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