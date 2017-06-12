#pragma once
#include"LoginMainInfo.h"
#include"UserInfo.h"
#include"ConnectToServer.h"
#include<string>

/*	LoginTxtSize */
#define X_LOGIN_TXT_SIZE 58
#define Y_LOGIN_TXT_SIZE 18
#define TXT_SPEED 1
class LoginMain
{
private:
	char loginTitleTxtArr[Y_LOGIN_TXT_SIZE][X_LOGIN_TXT_SIZE];
	int loginArr[5];//loginArrCode for UpDownCheck()
public:
	LoginMain();
	~LoginMain();
	void ConnectServer();
	int LoginMainStart();
	void setUserInfo(UserInfo*);
	ConnectToServer toServer;
private:
	LoginMainInfo lf;
	UserInfo *user;
	grideXY gride;
	int ReadLoginTitleTxt(); // �α���ȭ�鿡 ����� �ý�Ʈ ���� �б�	

	void PrintLoginTitleTxt();//�α���ȭ�鿡 �ҷ��� �ؽ�Ʈ ���� ���
	void PrintStartLeft(int y, int xTxtNum);//y�� ����� ���ڰ� �� ��° ���̳ĸ� �˸�
	void PrintStartRight(int y, int xTxtNum);

	void PrintLoginMainTxtList();
	void SelectServerTxtList();

	int KeyBoardCheck(const int key);
	const int InputKey();
	void UpDownCheck(const int key);
	void loginTxtCheckPrint(int loginArrCode);
	void initCheckPrint();

	void PrintUserID();
	void PrintUserPass();

	/*��й�ȣƲ���ų� �ƾƵ� �ߺ��ΰ��*/
	void PrintConnectErrorMsg(int ConnetErrorCode);

	/*Login Txt ����*/

};

