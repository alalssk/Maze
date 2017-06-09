#pragma once
#include<iostream>
#include"grideXY.h"

#define USER_ID_SIZE 20//before 13
#define USER_PASS_SIZE 20
class LoginMainInfo
{
private:
	int LoginArrNum;
	int serverNum;
	/*user 부분*/
	char userID[USER_ID_SIZE];
	int inputIdSz;//입력된 ID 넘버(개 수)

	char userPass[USER_PASS_SIZE];
	int inputPassSz;//입력된 패스워드 넘버
public:
	LoginMainInfo();
	~LoginMainInfo();
	void setLoginArrPlus();
	void setLoginArrMinus();
	int getLoginArrNum();

	void setServerPlus();
	void setServerMinus();
	int getServerNum();

	int cInputUserID(char key);
	int getUserIdSz();
	char* getUserId();

	int cInputUserPass(char key);
	int getUserPassSz();
	char* getUserPass();
	void initID_Pass();
	void initLoginArr(); //LoginArrNum 맴버 초기화

};

