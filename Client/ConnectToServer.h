#pragma once
#include<iostream>
#include<windows.h>
#include<process.h>
#include<string>
#include"Lobby.h"
using namespace std;
#define BUF_SIZE 1024
#define ID_PASS_SIZE 13+1+20 // Id_password
#define CONNECT_CODE_SIZE 20
class ConnectToServer
{
private:
	typedef struct
	{
		SOCKET sock;
		Lobby lobby;//오직 printWaitingRoom 함수만을 위해사용
	}ThreadData;
	WSADATA wsaData;
	SOCKET Sock;
	SOCKADDR_IN servAddr;
	HANDLE hSndThread, hRcvThread;
	ThreadData td;
	static bool threadOn;//임시
	//	char ID_Pass[ID_PASS_SIZE];
	static unsigned WINAPI RecvMsg(PVOID arg);
public:
	ConnectToServer();
	~ConnectToServer();

	bool setupSock();//WSAStartup ~ connect 까지 나중에 서버IP 선택or입력 할수있게 변경
	void CleanupSock();
	bool StartConnect(char* ID_PASS);//ID 비번을 서버에 전송&처리 해주는 함수
	const SOCKET getSocket();
	static bool ExitFlag, LogoutFlag;
	static int ClientMode;//로그인화면(0), 로비(1), 대기방(2), 게임중(3);

};