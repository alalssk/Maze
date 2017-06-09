#pragma once
#include<iostream>
#include<windows.h>
#include<process.h>
#include<string>
#include"Lobby.h"
using namespace std;
#define ID_PASS_SIZE 13+1+20 // Id_password
#define CONNECT_CODE_SIZE 20
class ConnectToServer
{
private:

	WSADATA wsaData;
	SOCKET Sock;
	SOCKADDR_IN servAddr;

public:
	ConnectToServer();
	 ~ConnectToServer();

	bool setupSock();//WSAStartup ~ connect 까지 나중에 서버IP 선택or입력 할수있게 변경
	void CleanupSock();
	bool StartConnect(char* ID_PASS);//ID 비번을 서버에 전송&처리 해주는 함수
	const SOCKET getSocket();
	

};