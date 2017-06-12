#pragma once
#include<iostream>
#include<Windows.h>
#include"grideXY.h"
/*User Infomation*/
#define USER_ID_SIZE 13
#define USER_PASS_SIZE 20
#define MAX_USERNUM 3
/*User Infomation*/
enum GameState
{
	LOGIN = 0,
	LOBBY,
	WAIT_ROOM,
	GAMEPLAY,
	GAMERESULT,
};
class UserInfo
{
	char id[USER_ID_SIZE];
	char password[USER_PASS_SIZE];
	char userID_Password_for_send[USER_ID_SIZE + USER_PASS_SIZE+1];
	int WinCount;
	int RoomUserKey;//1~3 1이면 방장
	SOCKET sock;

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
//	void initUserInfo();
	void setWaitingRoomData(char*);//ChangeRoomState();
	void setWaitingRoomUserList(char*);
	bool setRoomState(bool);
	bool getRoomState();
	void setClientMode(GameState );
//	GameState getClientMode();
	void setRoomUserKey();

	bool IsCurrentClientMode(GameState state);
	typedef struct RoomData
	{
		int RoomNum;
		char RoomName[40];
		bool UserState[3];//게임 시작 준비 판단
		char UserName[3][USER_ID_SIZE];
		int winCount[3];
		int Rating[3];
		int EndUserNum;
		int ConnectUserNum;
		//int x[3], y[3];
		void SetPositionXY(int key, int UserKey)
		{
			switch (key)
			{
			case UP:
				y[UserKey - 1] -= 1;
				break;
			case LEFT:
				x[UserKey - 1] -= 1;
				break;
			case RIGHT:
				x[UserKey - 1] += 1;
				break;
			case DOWN:
				y[UserKey - 1] += 1;
				break;
			}
		}
		void initUserXY()
		{
			x[0] = 2;
			y[0] = 2;
			x[1] = 2 + 59;
			y[1] = 2;
			x[2] = 2;
			y[2] = 2 + 27;
		}
		int GetUserX(int UserKey){ return x[UserKey - 1]; }
		int GetUserY(int UserKey){ return y[UserKey - 1]; }
	private:
	//	// UserNuM
		int x[3], y[3];
	}WaitingRoom_Data;
	GameState ClientMode;
	int GetRoomUserKey();
	void ExitWaitingRoom();

	WaitingRoom_Data wData;
};