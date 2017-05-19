// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"ServerClass.h"
#include"ServerDB.h"

int _tmain(int argc, _TCHAR* argv[])
{
	ServerDB sDB;
	sDB.StartDB();
	return 0;
}

