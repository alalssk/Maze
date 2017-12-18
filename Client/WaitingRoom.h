#pragma once
#include"WaitingRoomInfo.h"
#include"UserInfo.h"
#include<string>
#include<vector>
using CHAT_LOG = pair<string, string>;//name chat
class WaitingRoom
{
public:
	static HANDLE hWaitingRoomEventForRequest;

	void setUserInfo(UserInfo*);
	WaitingRoom();
	~WaitingRoom();

	int WatingRoomMain();
	void PrintUserList();//#유저리스트
	/*chat*/
	bool InputChatLog(string name, string chat);
	void PrintChatLogList();
	void initChatListBox();
	/*chat*/
	void AllClear();
	void PrintStartGameMsg();
	void initWaitingRoom();
private:
	WaitingRoomInfo WRinfo;
	SOCKET sock;
	UserInfo *user;
	grideXY gride;
	vector<CHAT_LOG> ChatLog;
	int key;

	bool req_ExitWaitingRoom();//@E_[방번호]_[ID]
	void req_SendMsg(char*);

	void PrintUserListBox();	
	void PrintButton();
	void AllClearPrintLobbyTxtBox();

	



};

