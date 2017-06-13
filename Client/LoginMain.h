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

public:
	LoginMain();
	~LoginMain();
	ConnectToServer toServer;

	void ConnectServer();
	int LoginMainStart();
	void setUserInfo(UserInfo*);

private:
	char loginTitleTxtArr[Y_LOGIN_TXT_SIZE][X_LOGIN_TXT_SIZE];
	int loginArr[5];//loginArrCode for UpDownCheck()
	LoginMainInfo lf;
	UserInfo *user;
	grideXY gride;

	int ReadLoginTitleTxt(); // 로그인화면에 출력할 택스트 파일 읽기	

	void PrintLoginTitleTxt();//로그인화면에 불러온 텍스트 파일 출력
	void PrintStartLeft(int y, int xTxtNum);//y는 출력할 문자가 몇 번째 줄이냐를 알림
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

	/*비밀번호틀리거나 아아디 중복인경우*/
	void PrintConnectErrorMsg(int ConnetErrorCode);

	/*Login Txt 관련*/

};

