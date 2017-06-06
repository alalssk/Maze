#pragma once
#include<iostream>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>
#include<time.h>
#include<list>
#include<vector>
#include<string>
#include "LogClass.h"//log
#include"ServerDB.h"
using namespace std;
#define BUF_SIZE 100
/*IOCP MODE*/
#define READ		3
#define FIRST_READ	4
#define CREATE_ROOM 5
#define ROOM_READ	6
#define	WRITE		7

#define MAX_CLN_NUM 300
#define MAX_THR_NUM 3
#define MAX_NAME_SIZE 20
#define MAX_CHATROOM_SIZE 255

class ServerClass
{
	typedef struct    // socket info
	{
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		char name[MAX_NAME_SIZE];
		int win_count;
		int play_count;
		int MyRoom;
	}CLIENT_DATA, *LPCLIENT_DATA;

	typedef struct    // buffer info
	{
		OVERLAPPED overlapped;
		WSABUF wsaBuf;
		char buffer[BUF_SIZE];
		int Mode;    // READ or WRITE
	} OVER_DATA, *LPOVER_DATA;

	typedef struct//chat room info
	{
		int ChatRoomNum;//방 번호(고유)
		char chatRoomName[MAX_CHATROOM_SIZE];
		char ClientsID[3][MAX_NAME_SIZE];
		bool UserState[3];
		SOCKET hClntSock[3];
		int UserCount;//이 방에 접속중인 user수
	}ChatRoom, *LPChatRoom;

	static int ChatRoomCount;
	typedef struct		//iocp handle +@
	{
		//LPPER_IO_DATA ioInfo;
		SOCKET hServSock;
		SOCKADDR_IN servAdr;
		HANDLE hComPort;
		list<CLIENT_DATA> Clients; // list로 하면 어째선지 안됨 초기화를 하라던지...초기화를 하면 다른문제로....동적할당을 안해서 생긴 문제였음
		list<ChatRoom> ChatRoomList;
		list<ChatRoom> GameRoomList;
		int Clients_Num;//접속중인 클라 개수
		//SOCKET sClients[MAX_CLN_NUM];
		DWORD recvBytes, flags;
	} Shared_DATA, *LPShared_DATA;

	LPShared_DATA shareData;



	SYSTEM_INFO sysInfo;
	HANDLE hTheards[MAX_THR_NUM];
	WSADATA	wsaData;

	static bool ExitFlag;
	static int TotalConnectedClientCount;
	static int TotalCreateRoomCount;

	const SOCKET GetListenSock(const int Port, const int Backlog);
	static void CloseClientSock(SOCKET, LPOVER_DATA, LPShared_DATA);
	static const bool CreateRoomFunc(LPShared_DATA lpComp, SOCKET sock);
	static const bool ExitRoomFunc(LPShared_DATA lpComp, int RoomNum, char *id);//이 함수는 항상 cs안에있어야함
	static const bool JoinRoomFunc(LPShared_DATA lpComp, SOCKET sock, int RoomNum);
	static const bool SendNewWaitingUserList(list<ChatRoom>::iterator);
	static void SendMsgFunc(char* buf, LPShared_DATA lpComp, DWORD RecvSz);
	static void ServerClass::SendMsgWaitingRoomFunc(int RoomNum, LPShared_DATA ipComp, char* msg);
	static bool SendWaitingRoomList(LPShared_DATA lpComp);
	static bool SendUserState(LPShared_DATA lpComp, char *input);//[방번호]_[ID]
	static void SendUserInputKey_GamePlay(LPShared_DATA, int, int, int);
	static const bool SetStartRoom(LPShared_DATA lpComp, int RoomNum);
	static const bool DeleteStartRoom(LPShared_DATA lpComp, int RoomNum);
	static unsigned __stdcall AcceptThread(PVOID pServSock);
	bool Create_IOCP_ThreadPool();
	static unsigned  __stdcall IOCPWorkerThread(LPVOID CompletionPortIO);
	static CRITICAL_SECTION cs;
	//	void ErrorHandling(const char *message);


public:
	static LogClass Chatlog;
	static LogClass DBLog;
	static ServerDB sDB;
	ServerClass();
	~ServerClass();
	bool ServerClassMain();
	void printConnectClientNum();
	void Print_UserList();
	void Print_RoomList();
	void Print_GameRoomList();
	void ExitIOCP();
	void PrintRoomInfo();
};

