#pragma once
#include<windows.h>
#include<iostream>
#include<conio.h>
using namespace std;
#define X_MAX 119
#define Y_MAX 30
/*		key			*/
#define UP 0x48//72
#define DOWN 0x50 //80
#define LEFT 0x4b // 75
#define RIGHT 0x4d // 77
#define BACK_SPACE 0x08
#define SPACE 0x20 //32
#define ENTER 0x0a

class grideXY
{
protected:
	int x, y;
	char key;
	grideXY();
	virtual ~grideXY();
	void gotoxy(int, int);
	void ClearXY();//allclear
	void ClearXY(int);//y
	void ClearXY(int, int, int);//x,y,count
	void GrideBox(int x, int y, int h, int w);//x,y 는 좌표 h,w 는 박스의 높이 너비
	//	void oneLineClase();
	void txtLineClear();//20칸 초기화
	/*key check*/
	int getKeyDirectionCheck();
	//	int keyboardCheck(const int key, int * loginArrNum, LOGIN_INFO *login_info);

};

