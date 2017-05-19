#include "grideXY.h"

grideXY::grideXY()
{
	x = 0, y = 0;
}


grideXY::~grideXY()
{
	gotoxy(1, 1);
}

void grideXY::gotoxy(int x, int y)
{
	COORD Pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void grideXY::ClearXY()
{
	for (int i = 0; i < Y_MAX; i++)// MAX값 으로 바꾸자
	{
		gotoxy(1, i);
		cout << "                                                                                                                      ";
	}
	gotoxy(1, 1);
}
void grideXY::ClearXY(int y)
{
	gotoxy(1, y);
	cout << "                                                                                                                          ";

}
void grideXY::ClearXY(int x, int y, int len)
{
	gotoxy(x, y);
	for (int i = 0; i < len; i++)
		cout << ' ';
}

int grideXY::getKeyDirectionCheck()//키입력
{
	char ch;
	char key;

	ch = _getch();//conio.h
	if (ch == 0 || ch == 224)
	{
		key = _getch();
		if (key == UP) return UP;
		else if (key == DOWN) return DOWN;
		else if (key == LEFT) return LEFT;
		else if (key == RIGHT) return RIGHT;
		//else return -1;
	}
	else
	{
		return ch;
	}
}
void grideXY::txtLineClear()//한 줄 50칸 출력 텍스트 초기화
{
	cout << "                                                  ";
}
void grideXY::GrideBox(int x, int y, int h, int w)
{
	int i = 0;
	gotoxy(x, y);
	cout << "┌"; for (int j = 0; j < w; j++)cout << "─"; cout << "┐" << endl;
	for (i = 1; i <= h; i++)
	{
		gotoxy(x, y + i);
		cout << "│"; for (int j = 0; j < w; j++)cout << "  "; cout << "│" << endl;
	}
	gotoxy(x, y + i);
	cout << "└"; for (int j = 0; j < w; j++)cout << "─"; cout << "┘" << endl;
}
