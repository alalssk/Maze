#pragma once
#include<iostream>
#include <process.h>
#include <WinSock2.h>
#include <Windows.h>
//#include <stdatomic.h> //atomic ����
#include<atomic>
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
#define MAX_USERNUM 3
enum PacketType
{
	REQUEST_CREATE_ROOM=0,
	REQUEST_ROOM_LIST,
	REQUEST_JOIN_ROOM,
	REQUEST_EXIT_ROOM,
	REQUEST_LOGOUT,
	REQUEST_GAME_EXIT,
	ROOM_CHAT,
	REQUEST_START_GAME,
	REQUEST_READY_GAME,
	MOVE,
	USER_GAME_FINISH,
	GAME_OVER,
	NONE,
	TEST_PACKET
};
unsigned int __stdcall AcceptThread(PVOID pServSock);
unsigned  __stdcall IOCPWorkerThread(LPVOID CompletionPortIO);
class ServerClass
{
public:
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
		int ChatRoomNum;//�� ��ȣ(����)
		char chatRoomName[MAX_CHATROOM_SIZE];
		char ClientsID[MAX_USERNUM][MAX_NAME_SIZE];
		bool UserState[MAX_USERNUM];
		SOCKET hClntSock[MAX_USERNUM];
		int UserCount;//�� �濡 �������� user��
		int gameID; //playgame���� ��

	}ChatRoom, *LPChatRoom;

	typedef struct		//iocp handle +@
	{
		//LPPER_IO_DATA ioInfo;
		SOCKET hServSock;
		SOCKADDR_IN servAdr;
		HANDLE hComPort;
		list<CLIENT_DATA> Clients; // list�� �ϸ� ��°���� �ȵ� �ʱ�ȭ�� �϶����...�ʱ�ȭ�� �ϸ� �ٸ�������....�����Ҵ��� ���ؼ� ���� ��������
		list<ChatRoom> ChatRoomList;
		list<ChatRoom> GameRoomList;
		int Clients_Num;//�������� Ŭ�� ����
		//SOCKET sClients[MAX_CLN_NUM];
		DWORD recvBytes, flags;
		ServerClass *thisServerClass;
	} Shared_DATA, *LPShared_DATA;

	LPShared_DATA shareData;



	SYSTEM_INFO sysInfo;
	HANDLE hTheards[MAX_THR_NUM];
	WSADATA	wsaData;

	static bool ExitFlag;
	static int TotalConnectedClientCount;
	static int TotalCreateRoomCount;

	const SOCKET GetListenSock(const int Port, const int Backlog);
	 bool CloseClientSock(SOCKET, LPOVER_DATA, LPShared_DATA);
	 const bool CreateRoomFunc(LPShared_DATA lpComp, SOCKET sock);
	 const bool ExitRoomFunc(LPShared_DATA lpComp, int RoomNum, char *id);//�� �Լ��� �׻� cs�ȿ��־����
	 const bool JoinRoomFunc(LPShared_DATA lpComp, SOCKET sock, int RoomNum);
	 const bool SendNewWaitingUserList(list<ChatRoom>::iterator);
	 void SendMsgFunc(char* buf, LPShared_DATA lpComp, DWORD RecvSz);
	 void ServerClass::SendMsgWaitingRoomFunc(int RoomNum, LPShared_DATA ipComp, char* msg);
	 bool SendWaitingRoomList(LPShared_DATA lpComp);
	 bool SendUserState(LPShared_DATA lpComp, char *input);//[���ȣ]_[ID]
	 void SendUserInputKey_GamePlay(LPShared_DATA, int, int, int);
	 const bool SetStartRoom(LPShared_DATA lpComp, int RoomNum);
	 const bool DeleteStartRoom(LPShared_DATA lpComp, int RoomNum);
	 const bool PlusWinCount(LPShared_DATA lpComp, SOCKET sock);//1���� ������ ��Ĺ�� ���ڷ�
	 
	bool Create_IOCP_ThreadPool();
	 
	 static CRITICAL_SECTION cs;
	//	void ErrorHandling(const char *message);

	 PacketType GetPacketTypeFromClient(char* buffer);

public:

	static LogClass Chatlog;
	static LogClass DBLog;
	ServerDB sDB;
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

