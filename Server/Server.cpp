// Server.cpp : Defines the entry point for the console application.
//

#include"ServerClass.h"
#include"ServerDB.h"

int main()
{


	ServerClass sc;
	SYSTEMTIME sTime;


	sc.ServerClassMain();
	int input;
	bool exitFlag = false;
	char cTime[25] = "";
	while (!exitFlag)
	{
		cout << "접속중인 클라 수 보기(1) 대기방출력(2) 대기방정보(3) 게임중방검색(4) 종료는 (5)" << endl;
		cin >> input;
		switch (input)
		{
		case 1:
			sc.printConnectClientNum();
			sc.Print_UserList();
			break;
		case 2:
			sc.Print_RoomList();
			break;
		case 3:
			sc.PrintRoomInfo();
			break;
		case 4:
			sc.Print_GameRoomList();
			break;
		case 5:
			exitFlag = true;
			break;
		case 6:
			
		//	GetLocalTime(&sTime);
		//	sprintf(cTime, "%d-%d-%d %d:%d:%d", sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond);
		//	cout << cTime << endl;
			//cout << sTime.wYear << '-' << sTime.wMonth << '-' << sTime.wDay << ' '<<sTime.wHour<<':'<<sTime.wMinute<<':'<<sTime.wSecond<<endl;
			break;
		default:
			break;
		}
		

	}
	return 0;
}

