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
		cout << "�������� Ŭ�� �� ����(1) �������(2) ��������(3) �����߹�˻�(4) ����� (5)" << endl;
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

