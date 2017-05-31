#pragma once
#include<iostream>
#include<Windows.h>
/*User Infomation*/
#define USER_ID_SIZE 13
#define USER_PASS_SIZE 20
/*User Infomation*/

class UserInfo
{
	char id[USER_ID_SIZE];
	char password[USER_PASS_SIZE];
	char userID_Password_for_send[USER_ID_SIZE + USER_PASS_SIZE+1];
	int WinCount;

	SOCKET sock;
	int state;//login(0), Lobby(1), WaitingRoom(2), PlayGame(3)



	bool RoomState;//방없으면 false



public:
	UserInfo();
	~UserInfo();
	void initUserInfoData();
	bool setID(char*);
	char* getID();
	bool setPassword(char*);
	char* getPassword();
	void setSocket(SOCKET sock);
	SOCKET getSocket();
	UserInfo getUserInfoClass();
	char* conv_ID_Password();
	void initUserInfo();
	void setWaitingRoomData(char*);//ChangeRoomState();
	void setWaitingRoomUserList(char*);
	bool setRoomState(bool);
	bool getRoomState();

	typedef struct RoomData
	{
		int RoomNum;
		char RoomName[40];
		bool state[3];//일단 안씀
		char UserName[3][USER_ID_SIZE];
		int winCount[3];
		int ConnectUserNum;
	}WaitingRoom_Data;
	WaitingRoom_Data wData;

	void ExitWaitingRoom();

};