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



	//	char ID_Pass[ID_PASS_SIZE];

public:
	ConnectToServer();
	 ~ConnectToServer();

	bool setupSock();//WSAStartup ~ connect ���� ���߿� ����IP ����or�Է� �Ҽ��ְ� ����
	void CleanupSock();
	bool StartConnect(char* ID_PASS);//ID ����� ������ ����&ó�� ���ִ� �Լ�
	const SOCKET getSocket();
	

};