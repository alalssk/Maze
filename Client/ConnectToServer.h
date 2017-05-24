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
		Lobby lobby;//���� printWaitingRoom �Լ����� ���ػ��
	}ThreadData;
	WSADATA wsaData;
	SOCKET Sock;
	SOCKADDR_IN servAddr;
	HANDLE hSndThread, hRcvThread;
	ThreadData td;
	static bool threadOn;//�ӽ�
	//	char ID_Pass[ID_PASS_SIZE];
	static unsigned WINAPI RecvMsg(PVOID arg);
public:
	ConnectToServer();
	~ConnectToServer();

	bool setupSock();//WSAStartup ~ connect ���� ���߿� ����IP ����or�Է� �Ҽ��ְ� ����
	void CleanupSock();
	bool StartConnect(char* ID_PASS);//ID ����� ������ ����&ó�� ���ִ� �Լ�
	const SOCKET getSocket();
	static bool ExitFlag, LogoutFlag;
	static int ClientMode;//�α���ȭ��(0), �κ�(1), ����(2), ������(3);

};