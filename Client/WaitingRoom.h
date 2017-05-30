#pragma once
#include"WaitingRoomInfo.h"
#include"UserInfo.h"
#include<string>
#include<vector>
using CHAT_LOG = pair<string, string>;//name chat
class WaitingRoom :protected grideXY
{
public:
	static HANDLE hWaitingRoomEventForRequest;
	//void req_GetWaitingRoomInfo();//방정보를 요청하고 응답은 recv스레드에서 받아 userInfo의 방정보에 넣는다
	//void req_GetWaitingRoomUserInfo();
	//void req_ChattingSendToServer(string chat);
	void setUserInfo(UserInfo*);
	WaitingRoom();
	~WaitingRoom();
	//int WatingRoomMain(SOCKET sock);
	int WatingRoomMain();
	void PrintUserList();//#유저리스트
private:
	bool req_ExitWaitingRoom();//@E_[방번호]_[ID]

	void PrintUserListBox();
	
	void PrintButton();
	void AllClearPrintLobbyTxtBox();
	WaitingRoomInfo WRinfo;
	SOCKET sock;
	UserInfo *user;
	vector<CHAT_LOG> ChatLog;
	//UserInfo user;
	bool InputChatLog(string name, string chat);
	void PrintChatLogList();
	void initChatListBox();

};

