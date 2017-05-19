#pragma once
#include<iostream>
#include<windows.h>
#include<process.h>
#include<string>
using namespace std;
#define BUF_SIZE 100
#define ID_PASS_SIZE 13+1+20 // Id_password
#define CONNECT_CODE_SIZE 20
class ConnectToServer
{
private:
	WSADATA wsaData;
	SOCKET Sock;
	SOCKADDR_IN servAddr;
	HANDLE hSndThread, hRcvThread;
	//	char ID_Pass[ID_PASS_SIZE];
	char msg[BUF_SIZE];
public:
	ConnectToServer();
	~ConnectToServer();
	bool setupSock();
	bool StartConnect(char*);
};