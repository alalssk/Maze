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
	user->wData.x[0] = 2;
	user->wData.y[0] = 2;
	user->wData.x[1] = 2+59;
	user->wData.y[1] = 2;
	user->wData.x[2] = 2;
	user->wData.y[2] = 2+27;

	ClearXY();
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
		ClearXY();

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

	//ClearXY(); info.PrintThree(); Sleep(1500);
	//ClearXY(); info.PrintTwo(); Sleep(1500);
	ClearXY(); info.PrintOne(); Sleep(1000);

	gotoxy(1, 1);
	info.grideMap();
	gotoxy(user->wData.x[0], user->wData.y[0]); cout << '*';
	gotoxy(user->wData.x[1], user->wData.y[1]); cout << '*';
	gotoxy(user->wData.x[2], user->wData.y[2]); cout << '*';
	while (1)
	{


		gotoxy(1, 1);
		//move_arrow(getKeyDirectionCheck());//�ڵ�Ž�� ���鋚 getKey...�Լ� ���� �ڵ����� Ű�Է��ϴ� �Լ� ����� �ɵ�
		SendInputKey(getKeyDirectionCheck());
		//���⼭ ���� ��ǥ send
		if (finalPoint())//��������
		{
			ClearXY();
			//	Ending();
			//userlog.PrintUserlog();
			user->wData.x[0] = 2;
			user->wData.y[0] = 2;
			user->wData.x[1] = 2 + 60;
			user->wData.y[1] = 2;
			user->wData.x[2] = 2;
			user->wData.y[2] = 2 + 28;
			return 0;
		}
		else
		{

		}
	}

}

void GamePlayClass::initPrint()
{
	gotoxy(64, 20);
	cout << "           ";
}
void GamePlayClass::movePrint()
{
	gotoxy(64, 20);
	cout << " ���Դϴ� ";
}
void GamePlayClass::Ending()
{
	string cong = "�����մϴ�!!!";
	string end_comment = "������ ���� �մϴ�!!!";
	gotoxy(X_MAX / 2 - cong.size() / 2 - 1, Y_MAX / 2), cout << cong;
	for (int i = 1; i < Y_MAX / 2 + 1; i++)
	{
		EndingSubLoop(i, '#');
		Sleep(100);
	}

	for (int i = Y_MAX / 2 + 1; i >= 1; i--)
	{
		if (i > 3)
		{
			gotoxy(X_MAX / 2 - end_comment.size() / 2 - 1, Y_MAX / 2), cout << end_comment;
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
		gotoxy(i, a), cout << c;
		//	Sleep(SPEED);
	}

	for (j = 0 + a; j < Y_MAX - a; j++)
	{
		gotoxy(i - 1, j), cout << c;
		//		Sleep(SPEED);
	}
	for (; i > 0 + a; i--)
	{
		gotoxy(i - 1, j), cout << c;
		//		Sleep(SPEED);
	}
	for (; j > 0 + a; j--)
	{
		gotoxy(i, j), cout << c;
		//		Sleep(SPEED);
	}
	gotoxy(45, 30);
}
bool GamePlayClass::finalPoint()
{
	if (info.gameMap[user->wData.y[0] - 1][user->wData.x[0] - 1] == 'F') return TRUE;
	else return FALSE;
}

bool GamePlayClass::SendInputKey(int inputKey)
{// P���ȣ_����Ű_����Ű
	initPrint();//�ӽ�
	char SendMsg[13] = "";
	switch (inputKey)
	{
	case UP://����
		if (info.gameMap[user->wData.y[MyKey - 1] - 2][user->wData.x[MyKey - 1] - 1] == '#') {
			//��
			return false;
		}
		else {
			//gotoxy(x, y); cout << ' ';
			//y -= 1;//�ӽ÷� �س���� ���⼭ ���� �������� ��Ŷ �޾����� �ٲ������
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), inputKey);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	case LEFT: //����
		if (info.gameMap[user->wData.y[MyKey - 1] - 1][user->wData.x[MyKey - 1] - 2] == '#'){
			//��
			return false;
		}
		else {
			//gotoxy(x, y); cout << ' ';
			//x -= 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), inputKey);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	case RIGHT: //������
		if (info.gameMap[user->wData.y[MyKey - 1] - 1][user->wData.x[MyKey - 1]] == '#'){
			//��
			return false;
		}
		else {
			//gotoxy(x, y); cout << ' ';
			//x += 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), inputKey);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	case DOWN: //�Ʒ�
		if (info.gameMap[user->wData.y[MyKey - 1]][user->wData.x[MyKey - 1] - 1] == '#'){
			//��
			return false;
		}
		else {
			//gotoxy(x, y); cout << ' ';
			//y += 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), inputKey);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	default:
		gotoxy(64, 1);
		cout << "����Ű���Է� (�����q)" << endl;
		return false;
	}

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

	switch (InputKey)
	{
	case UP://����

		gotoxy(user->wData.x[UserKey - 1], user->wData.y[UserKey - 1]); cout << ' ';
		user->wData.y[UserKey - 1] -= 1;
		break;
	case LEFT: //����

		gotoxy(user->wData.x[UserKey - 1], user->wData.y[UserKey - 1]); cout << ' ';
		user->wData.x[UserKey - 1] -= 1;
		break;
	case RIGHT: //������

		gotoxy(user->wData.x[UserKey - 1], user->wData.y[UserKey - 1]); cout << ' ';
		user->wData.x[UserKey - 1] += 1;
		break;
	case DOWN: //�Ʒ�

		gotoxy(user->wData.x[UserKey - 1], user->wData.y[UserKey - 1]); cout << ' ';
		user->wData.y[UserKey - 1] += 1;
		break;
	}
	gotoxy(user->wData.x[UserKey - 1], user->wData.y[UserKey - 1]); cout << '*';
}