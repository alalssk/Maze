// Server.cpp : Defines the entry point for the console application.
//

#include"ServerClass.h"
#include"ServerDB.h"

int main()
{
	ServerClass sc;
	sc.ServerClassMain();
	int input;
	bool exitFlag = false;
	while (!exitFlag)
	{
		cout << "접속중인 클라 수 보기(1) 대기방출력(2) 대기방정보(3) 종료는 (5)" << endl;
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
		case 5:
			exitFlag = true;
			break;
		}
		

	}
	return 0;
}

