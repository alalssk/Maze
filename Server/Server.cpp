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
		cout << "Á¾·á´Â 5";
		cin >> input;
		switch (input)
		{
		case 1:
			sc.printConnectClientNum();
			break;
		case 5:
			exitFlag = true;
			break;
		}
		

	}
	return 0;
}

