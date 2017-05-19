// Server.cpp : Defines the entry point for the console application.
//

#include"ServerClass.h"
#include"ServerDB.h"

int main()
{
	ServerClass sc;
	sc.ServerClassMain();
	int input;
	while (1)
	{
		cout << "Á¾·á´Â 5";
		cin >> input;
		if (input == 5) break;

	}
	return 0;
}

