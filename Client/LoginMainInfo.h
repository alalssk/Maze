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
	/*user �κ�*/
	char userID[USER_ID_SIZE];
	int inputIdSz;//�Էµ� ID �ѹ�(�� ��)

	char userPass[USER_PASS_SIZE];
	int inputPassSz;//�Էµ� �н����� �ѹ�
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
	void initLoginArr(); //LoginArrNum �ɹ� �ʱ�ȭ

};

