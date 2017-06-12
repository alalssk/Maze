#include "LoginMain.h"


LoginMain::LoginMain()
{
	loginArr[0] = 0;
	loginArr[1] = 2;
	loginArr[2] = 4;
	loginArr[3] = 6;
	loginArr[4] = 8;
	///	loginArr = { 0, 2, 4, 6, 8 };
}


LoginMain::~LoginMain()
{
}
int LoginMain::LoginMainStart()
{

	gride.ClearXY();
	ReadLoginTitleTxt();
	PrintLoginTitleTxt();
	PrintLoginMainTxtList();
	SelectServerTxtList();
	loginTxtCheckPrint(0);
	switch (InputKey())
	{//exit와 logout코드를 통합할 필요가 있는것 같군
	case 0://exit
		gride.ClearXY();
		//clientExit();
		return 2;//EXIT_CODE 메인(client.cpp)에서 2임

	case 1://connect
		cout << "접속완료" << endl;
		return 1;
	}
}
void LoginMain::setUserInfo(UserInfo *input_user)
{
	user =input_user;
}
int LoginMain::ReadLoginTitleTxt()//x:57, y:18
{
	char buf[X_LOGIN_TXT_SIZE + 1];
	int len, i = 0;
	int count = 0;//
	FILE *readLoginTxtFilePointer = fopen("loginTxt.txt", "rt");
	if ((readLoginTxtFilePointer = fopen("loginTxt.txt", "r")) == NULL){
		perror("loginTxt.txt: ");
		return -1;
	}

	while (1)
	{
		fgets(buf, X_LOGIN_TXT_SIZE + 1, readLoginTxtFilePointer);
		len = strlen(buf);

		if (len == 0) break;
		strcpy(loginTitleTxtArr[i], buf); i++;
		buf[0] = '\0';
		count++;
	}

	fclose(readLoginTxtFilePointer);
	return count;
}
void LoginMain::PrintLoginTitleTxt()
{
	//gotoxy(10, 10); //임시

	for (int i = 57 + 1; i > 0; i--)
	{
		PrintStartLeft(0, i);
		Sleep(TXT_SPEED);
	}
	for (int i = 0; i < 57 + 1; i++)//	for (int j = 0; j<57 + 1; j++)
	{
		PrintStartRight(9, i);
		Sleep(TXT_SPEED);
	}

}

void LoginMain::PrintStartLeft(int y, int xTxtNum)
//y는 출력할 문자가 몇 번째 줄이냐를 알림
{
	for (int i = 0; i < 9; i++)
	{
		gride.gotoxy(25, 3 + i);
		for (int j = xTxtNum; j < X_LOGIN_TXT_SIZE + 1; j++)
		{
			cout << loginTitleTxtArr[y][j];
		}
		y++;
		cout << endl;
	}
}
void LoginMain::PrintStartRight(int y, int xTxtNum)
{

	for (int i = 0; i < 9; i++)
	{
		gride.gotoxy(25 + X_LOGIN_TXT_SIZE - xTxtNum, 3 + i + 9);
		for (int j = 0; j < xTxtNum; j++)
		{
			cout << loginTitleTxtArr[y][j];
		}
		y++;
		cout << endl;
	}
}

void LoginMain::PrintLoginMainTxtList()
{
	gride.DrawXY(36, 23, "1. 서버선택: ");
	gride.DrawXY(36, 25, "2.    ID   : ");
	gride.DrawXY(36, 27, "3. PassWord: ");
	gride.DrawXY(36 + 10, 29, "[ 접 속 하 기 ]");
	gride.DrawXY(36 + 10, 31, "[ 종 료 하 기 ]");
}
void LoginMain::SelectServerTxtList()
{
	switch (lf.getServerNum())
	{
	case 0:
		gride.DrawXY(54, 23, "◀  서  버   1  ▶");
		break;
	case 1:
		gride.DrawXY(54, 23, "◀  서  버   2  ▶");
		break;
	case 2:
		gride.DrawXY(54, 23, "◀  서  버   3  ▶");
		break;
	}
}

const int LoginMain::InputKey()
{
	int tmp;
	while (1)
	{
		gride.key = gride.getKeyDirectionCheck();
		tmp = KeyBoardCheck(gride.key);//return -> 종료하기(0) 루프계속(5)  접속 성공(1)
		if (tmp != 5)break; //대기방에서 로그인화면으로 돌아가기 만들자
	}
	return tmp;
}
int LoginMain::KeyBoardCheck(const int key) //return -> 종료하기(0), 루프계속(5), 접속 성공(1)
{
	if (key == UP || key == DOWN)
	{
		UpDownCheck(key);
	}
	else if (lf.getLoginArrNum() == 0)
	{
		if (key == LEFT || key == RIGHT)
		{
			if (key == LEFT) lf.setServerMinus();
			else if (key == RIGHT) lf.setServerPlus();
			SelectServerTxtList();
		}
	}
	else if (lf.getLoginArrNum() == 1) //ID 입력 부분
	{
		if (key == LEFT || key == RIGHT){}//ID, Password 에 좌 우 방향키가 입력되지 않게하기 위한 방법
		else
		{
			lf.cInputUserID(key);// ID 13개 Pass20개 이상 입력 안되게 하는 기능 추가
			PrintUserID();
		}
	}
	else if (lf.getLoginArrNum() == 2)
	{
		if (key == LEFT || key == RIGHT){}
		else
		{
			lf.cInputUserPass(key);
			PrintUserPass();
		}
	}
	else if (lf.getLoginArrNum() == 3) //접속하기 접속 실패시ErrorMsg출력, 성공시: return(1)
	{
		if (key == SPACE || key == ENTER)
		{
			char send_id_pass[20 + 20] = "";
			sprintf(send_id_pass, "%s_%s", lf.getUserId(), lf.getUserPass());
			if (toServer.StartConnect(send_id_pass))
			{
				user->setID(lf.getUserId());
				user->setPassword(lf.getUserPass());
				user->setSocket(toServer.getSocket());
				return 1;
			}
			else
			{
				gride.gotoxy(35, 29);
				cout << send_id_pass << "::";
				Sleep(1000);
				PrintConnectErrorMsg(0);
				return 5;
			}
		}
	}
	else if (lf.getLoginArrNum() == 4) //종료하기
	{
		if (key == SPACE || key == ENTER) return 0;
	}
	return 5;
}

void LoginMain::UpDownCheck(int key)
{
	if (key == UP)
	{
		lf.setLoginArrMinus();
		loginTxtCheckPrint(loginArr[lf.getLoginArrNum()]);
	}
	else if (key == DOWN)
	{
		lf.setLoginArrPlus();
		loginTxtCheckPrint(loginArr[lf.getLoginArrNum()]);
	}
}
void LoginMain::loginTxtCheckPrint(int loginArrCode)
{
	initCheckPrint();
	gride.DrawXY(32, 23 + loginArrCode, "☞ ");
}//y 초기화 시켜줘야함 initCheckPrint()

void LoginMain::initCheckPrint()
{
	for (int i = 0; i < 5; i++)
	{
		gride.DrawXY(32, 23 + i * 2, "   ");
	}
}
void LoginMain::PrintUserID()
{
	gride.gotoxy(36 + 13, 25);	gride.txtLineClear();
	gride.gotoxy(36 + 13, 25);	cout << lf.getUserId();
}
void LoginMain::PrintUserPass()
{
	gride.gotoxy(36 + 13, 27);	gride.txtLineClear();
	gride.gotoxy(36 + 13, 27);
	for (int i = 0; i < lf.getUserPassSz(); i++)
		cout << "*";
}
void LoginMain::PrintConnectErrorMsg(int ConnetErrorCode)//0이면 id pass, 1이면 서버이상 에러 메시지 출력
{
	if (ConnetErrorCode == 0)
	{
		gride.DrawXY(35, 29, "- 중복된 ID 이거나 틀린 비밀번호 입니다 -");
		//gride.gotoxy(35, 29);		cout << "- 중복된 ID 이거나 틀린 비밀번호 입니다 -";
	}
	else {
		gride.DrawXY(36 + 6, 29, "- 서버 접속 오류 입니다 -");
		//gride.gotoxy(36 + 6, 29);		cout << "- 서버 접속 오류 입니다 -";
	}
	Sleep(1500);
	gride.gotoxy(32, 29);		gride.txtLineClear();
	PrintLoginMainTxtList();
	loginTxtCheckPrint(6);
}

void LoginMain::ConnectServer()
{
	while(!toServer.setupSock())	{}
}