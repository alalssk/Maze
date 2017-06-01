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
	//여기왔을때 상태변경요청send "$R[방번호]_[ID] ID말고 번호로 하면 어떨까? ---> 서버에서 상태 바꾸고 $R1_[ID]전송 클라에서는 해당 ID상태 바꿔줌
	//0.5초마다 세명의 상태를 비교하여 보두 true가 되면 3초후 게임시작 
	info.ReadMap();
	gotoxy(1, 1);
	info.grideMap();
	while (1)
	{

		gotoxy(x, y), cout << '*';//플레이어 표시(임시)
		gotoxy(1, 1);
		move_arrow(getKeyDirectionCheck());
		if (finalPoint())//골인지점
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
	cout << " 벽입니다 ";
}
void GamePlayClass::Ending()
{
	string cong = "축하합니다!!!";
	string end_comment = "게임을 종료 합니다!!!";
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
void GamePlayClass::move_arrow(int key)//x,y 플레이어 좌표값 (임시)
{
	initPrint();

	switch (key)
	{
	case UP://위로
		if (info.gameMap[y - 2][x - 1] == '#') {
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			y -= 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case LEFT: //왼쪽
		if (info.gameMap[y - 1][x - 2] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			x -= 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case RIGHT: //오른쪽
		if (info.gameMap[y - 1][x] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			x += 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case DOWN: //아래
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
		cout << "방향키만입력 (종료는q)" << endl;
		return;
	}
}