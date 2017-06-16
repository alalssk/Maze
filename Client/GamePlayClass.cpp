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
	//��������� ���º����ûsend "$S[���ȣ]_[ID] ID���� ��ȣ�� �ϸ� ���? 
	//�������� ���� �ٲٰ� $S1_[ID]����(�������� �ش��ϴ� �� ��ȣ�� Ŭ�󿡸� �������ִ°Ŵϱ� ���ȣ�� ������ �ʿ����) Ŭ�󿡼��� �ش� ID���� �ٲ���
	//0.5�ʸ��� ������ ���¸� ���Ͽ� ���� true�� �Ǹ� 3���� ���ӽ��� 
	//���� ���ӽ��� �غ� �Ǿ��� >> $S[���ȣ]_[ID] �� ������ ����
	sprintf(SendMsg, "$S%d_%s", user->wData.RoomNum, user->getID());//$S[���ȣ]_[ID]
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
		cout << "�� ��ȣ [" << user->wData.RoomNum << ']' << endl;
		cout << "�� �̸�: " << user->wData.RoomName << endl;
		cout << "�濡 �������� ���� ��: " << user->wData.ConnectUserNum << endl;
		for (int i = 0; i < user->wData.ConnectUserNum; i++)
		{
			cout << i << ". ID: " << user->wData.UserName[i] << " [�¼�-" << user->wData.winCount[i] << "] ����(";
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
		//���⼭ ���� ��ǥ send

		if (finalPoint())//�������� �ƾ� �ٲ�� �Ѵ�.
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
						cout << ">>������"; 
					else cout << user->wData.Rating[i] << "   ��";

				}
				Sleep(500);
			}
			if (user->wData.Rating[MyKey-1] == 1)
			{//�ϴ� �ӽ÷� 1� ����������Ŷ�� ������ ��������
				memset(SendMsg, 0, sizeof(SendMsg));
				sprintf(SendMsg, "q%d",user->wData.RoomNum);//������ "q���ȣ" �� �������� ���߿��� "q�ݹ�ȣ_�ɸ��ð�(������ ���ӳ����´� �ɸ��ð�)"�� ������
				send(sock, SendMsg, strlen(SendMsg) + 1, 0);
			}
			/*Ŭ���ʱ�ȭ*/
			gride.ClearXY();
			user->wData.initUserXY();
			for (int i = 0; i < user->wData.ConnectUserNum; i++)
			{
				user->wData.UserState[i] = false;
				user->wData.Rating[i] = 0;
			}
			user->wData.EndUserNum = 0;//endUserNum �ʱ�ȭ
			/*Ŭ���ʱ�ȭ*/
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
	gride.DrawXY(64, 20, " ���Դϴ� ");
}
void GamePlayClass::Ending()
{
	string cong = "�����մϴ�!!!";
	string end_comment = "������ ���� �մϴ�!!!";
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
{// P���ȣ_����Ű_����Ű
	initPrint();//�ӽ�
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
{//����Ű_����Ű
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