#include "GamePlayClass.h"


GamePlayClass::GamePlayClass() :MyKey(0)
{
}
 


GamePlayClass::~GamePlayClass()
{
}

void GamePlayClass::setUserInfo(UserInfo *input_user)
{
	user = input_user;
}
int GamePlayClass::mazeGameMain()
{
	sock = user->getSocket();

	MyKey = user->GetRoomUserKey();
	user->wData.initUserXY();

	gride.ClearXY();
	info.ReadMap();

	bool startFlag = false;
	char SendMsg[1024] = "";
	//여기왔을때 상태변경요청send "$S[방번호]_[ID] ID말고 번호로 하면 어떨까? 
	//서버에서 상태 바꾸고 $S1_[ID]전송(서버에서 해당하는 방 번호의 클라에만 전송해주는거니까 방번호를 적어줄 필요없음) 클라에서는 해당 ID상태 바꿔줌
	//0.5초마다 세명의 상태를 비교하여 보두 true가 되면 3초후 게임시작 
	//나는 게임시작 준비가 되었다 >> $S[방번호]_[ID] 을 서버로 보냄
	sprintf(SendMsg, "$S%d_%s", user->wData.RoomNum, user->getID());//$S[방번호]_[ID]
	send(sock, SendMsg, strlen(SendMsg) + 1, 0);
	while (!startFlag)
	{
		gride.ClearXY();

		startFlag = true;
		for (int i = 0; i < user->wData.ConnectUserNum; i++)
		{
			startFlag = startFlag & (user->wData.UserState[i]);
		}
		cout << "StartFlag : ";
		if (startFlag)cout << "TRUE" << endl; else cout << "FALSE" << endl;
		cout << "방 번호 [" << user->wData.RoomNum << ']' << endl;
		cout << "방 이름: " << user->wData.RoomName << endl;
		cout << "방에 접속중인 유저 수: " << user->wData.ConnectUserNum << endl;
		for (int i = 0; i < user->wData.ConnectUserNum; i++)
		{
			cout << i << ". ID: " << user->wData.UserName[i] << " [승수-" << user->wData.winCount[i] << "] 상태(";
			if (user->wData.UserState[i])cout << "TRUE)" << endl; else cout << "FALSE)" << endl;;
		}
		Sleep(1000);
	}

	gride.ClearXY(); info.PrintThree(); Sleep(1500);
	gride.ClearXY(); info.PrintTwo(); Sleep(1500);
	gride.ClearXY(); info.PrintOne(); Sleep(1000);

	gride.gotoxy(1, 1);
	info.grideMap();
	gride.DrawXY(user->wData.GetUserX(1), user->wData.GetUserY(1), "*");
	gride.DrawXY(user->wData.GetUserX(2), user->wData.GetUserY(2), "*");
	gride.DrawXY(user->wData.GetUserX(3), user->wData.GetUserY(3), "*");

	while (1)
	{


		gride.gotoxy(1, 1);
		//if (MyKey == 1)SendInputKey(getKeyDirectionCheck());
		//else SendInputKey(GetRandomKey());

		SendInputKey(gride.getKeyDirectionCheck());
		//여기서 나의 좌표 send

		if (finalPoint())//골인지점 아얘 바꿔야 한다.
		{
			char SendInputKeyMsg[20] = "";
			sprintf(SendInputKeyMsg, "Q%d_%d", user->wData.RoomNum, MyKey);
			send(sock, SendInputKeyMsg, strlen(SendInputKeyMsg) + 1, 0);

			while (user->wData.EndUserNum < user->wData.ConnectUserNum) 
			{

				for (int i = 0; i < user->wData.ConnectUserNum; i++)
				{
					//gotoxy(64 + 13, 20 + i); cout << "           ";
					gride.DrawXY(64 + 13, 20 + i, "           ");
				}
				for (int i = 0; i < user->wData.ConnectUserNum; i++)
				{
					//gotoxy(64, 20 + i); cout<< user->wData.UserName[i]; 
					gride.DrawXY(64, 20 + i, user->wData.UserName[i]);
					gride.gotoxy(64 + 13, 20 + i);
					if (user->wData.Rating[i] == 0)
						cout << ">>진행중"; 
					else cout << user->wData.Rating[i] << "   등";

				}
				Sleep(500);
			}
			if (user->wData.Rating[MyKey-1] == 1)
			{//일단 임시로 1등만 게임종료패킷을 서버로 보내도록
				memset(SendMsg, 0, sizeof(SendMsg));
				sprintf(SendMsg, "q%d",user->wData.RoomNum);//지금은 "q방번호" 만 보내지만 나중에는 "q반번호_걸린시간(세명이 게임끝내는대 걸린시간)"을 보내자
				send(sock, SendMsg, strlen(SendMsg) + 1, 0);
			}
			/*클라초기화*/
			gride.ClearXY();
			user->wData.initUserXY();
			for (int i = 0; i < user->wData.ConnectUserNum; i++)
			{
				user->wData.UserState[i] = false;
				user->wData.Rating[i] = 0;
			}
			user->wData.EndUserNum = 0;//endUserNum 초기화
			/*클라초기화*/
			return 0;
		}

	}

}

void GamePlayClass::initPrint()
{
	gride.DrawXY(64, 20, "           ");
}
void GamePlayClass::movePrint()
{
	gride.DrawXY(64, 20, " 벽입니다 ");
}
void GamePlayClass::Ending()
{
	string cong = "축하합니다!!!";
	string end_comment = "게임을 종료 합니다!!!";
	gride.gotoxy(X_MAX / 2 - cong.size() / 2 - 1, Y_MAX / 2), cout << cong;
	for (int i = 1; i < Y_MAX / 2 + 1; i++)
	{
		EndingSubLoop(i, '#');
		Sleep(100);
	}

	for (int i = Y_MAX / 2 + 1; i >= 1; i--)
	{
		if (i > 3)
		{
			gride.gotoxy(X_MAX / 2 - end_comment.size() / 2 - 1, Y_MAX / 2), cout << end_comment;
		}
		EndingSubLoop(i, ' ');
		Sleep(100);
	}

}
void GamePlayClass::EndingSubLoop(int a, char c)//ENDING
{
	int i, j;

	for (i = 0 + a; i < X_MAX - a; i++)
	{
		gride.gotoxy(i, a), cout << c;
		//	Sleep(SPEED);
	}

	for (j = 0 + a; j < Y_MAX - a; j++)
	{
		gride.gotoxy(i - 1, j), cout << c;
		//		Sleep(SPEED);
	}
	for (; i > 0 + a; i--)
	{
		gride.gotoxy(i - 1, j), cout << c;
		//		Sleep(SPEED);
	}
	for (; j > 0 + a; j--)
	{
		gride.gotoxy(i, j), cout << c;
		//		Sleep(SPEED);
	}
	gride.gotoxy(45, 30);
}
bool GamePlayClass::finalPoint()
{
	//info.GetGameMapValue(user) == 'F'
	if (info.gameMap[user->wData.GetUserY(MyKey) - 1][user->wData.GetUserX(MyKey) - 1] == 'F')
	{
		return TRUE;
	}
	else return FALSE;
}

bool GamePlayClass::SendInputKey(int inputKey)
{// P방번호_유저키_방향키
	initPrint();//임시
	char SendMsg[13] = "";
	switch (inputKey)
	{
	case UP:
		if (info.gameMap[user->wData.GetUserY(MyKey) - 2][user->wData.GetUserX(MyKey) - 1] == '#') return false;
		else break;
	case LEFT:
		if (info.gameMap[user->wData.GetUserY(MyKey) - 1][user->wData.GetUserX(MyKey) - 2] == '#') return false;
		else break;
	case RIGHT:
		if (info.gameMap[user->wData.GetUserY(MyKey) - 1][user->wData.GetUserX(MyKey)] == '#') return false;
		else break;
	case DOWN:
		if (info.gameMap[user->wData.GetUserY(MyKey)][user->wData.GetUserX(MyKey) - 1] == '#') return false;
		else break;
	default:
		return false;
	}
	sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, MyKey, inputKey);
	send(sock, SendMsg, strlen(SendMsg) + 1, 0);
	return true;
}
void GamePlayClass::RecvPlayerPosition(char* input)
{//유저키_방향키
	int UserKey, InputKey;
	char *tmp;
	tmp = strtok(input, "_");
	UserKey = atoi(tmp);
	tmp = strtok(NULL, "");
	InputKey = atoi(tmp);

	gride.DrawXY(user->wData.GetUserX(UserKey), user->wData.GetUserY(UserKey), " ");
	user->wData.SetPositionXY(InputKey, UserKey);
	gride.DrawXY(user->wData.GetUserX(UserKey), user->wData.GetUserY(UserKey), "*");
}

int GamePlayClass::GetRandomKey()
{
	Sleep(200);
	int key[4] = { UP, DOWN, LEFT, RIGHT };

	return key[rand() % 4];
}