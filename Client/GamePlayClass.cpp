#include "GamePlayClass.h"


GamePlayClass::GamePlayClass() :key(0), x(2), y(2)
{
}


GamePlayClass::~GamePlayClass()
{
}


int GamePlayClass::mazeGameMain()
{
	ClearXY();
	//��������� ���º����ûsend "$R[���ȣ]_[ID] ID���� ��ȣ�� �ϸ� ���? ---> �������� ���� �ٲٰ� $R1_[ID]���� Ŭ�󿡼��� �ش� ID���� �ٲ���
	//0.5�ʸ��� ������ ���¸� ���Ͽ� ���� true�� �Ǹ� 3���� ���ӽ��� 
	info.ReadMap();
	gotoxy(1, 1);
	info.grideMap();
	while (1)
	{

		gotoxy(x, y), cout << '*';//�÷��̾� ǥ��(�ӽ�)
		gotoxy(1, 1);
		move_arrow(getKeyDirectionCheck());
		if (finalPoint())//��������
		{
			ClearXY();
		//	Ending();
			//userlog.PrintUserlog();
			key = 0, x = 2, y = 2;//init
			return 0;
		}
		else
		{
			gotoxy(x, y);
			cout << '*';
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
	if (info.gameMap[y - 1][x - 1] == 'F') return TRUE;
	else return FALSE;
}
void GamePlayClass::move_arrow(int key)//x,y �÷��̾� ��ǥ�� (�ӽ�)
{
	initPrint();

	switch (key)
	{
	case UP://����
		if (info.gameMap[y - 2][x - 1] == '#') {
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			y -= 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case LEFT: //����
		if (info.gameMap[y - 1][x - 2] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			x -= 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case RIGHT: //������
		if (info.gameMap[y - 1][x] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			x += 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case DOWN: //�Ʒ�
		if (info.gameMap[y][x - 1] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			y += 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	default:
		gotoxy(64, 1);
		cout << "����Ű���Է� (�����q)" << endl;
		return;
	}
}