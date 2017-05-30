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
	//void req_GetWaitingRoomInfo();//�������� ��û�ϰ� ������ recv�����忡�� �޾� userInfo�� �������� �ִ´�
	//void req_GetWaitingRoomUserInfo();
	//void req_ChattingSendToServer(string chat);
	void setUserInfo(UserInfo*);
	WaitingRoom();
	~WaitingRoom();
	//int WatingRoomMain(SOCKET sock);
	int WatingRoomMain();
	void PrintUserList();//#��������Ʈ
private:
	bool req_ExitWaitingRoom();//@E_[���ȣ]_[ID]

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

