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
	//void req_GetWaitingRoomInfo();//�������� ��û�ϰ� ������ recv�����忡�� �޾� userInfo�� �������� �ִ´�
	//void req_GetWaitingRoomUserInfo();
	//void req_ChattingSendToServer(string chat);
	void setUserInfo(UserInfo*);
	WaitingRoom();
	~WaitingRoom();
	//int WatingRoomMain(SOCKET sock);
	int WatingRoomMain();
	void PrintUserList();//#��������Ʈ
	/*chat*/
	bool InputChatLog(string name, string chat);
	void PrintChatLogList();
	void initChatListBox();
	/*chat*/
	void AllClear();
	void PrintStartGameMsg();
	void initWaitingRoom();
private:
	bool req_ExitWaitingRoom();//@E_[���ȣ]_[ID]
	void req_SendMsgToServer(char*);
	void PrintUserListBox();
	
	void PrintButton();
	void AllClearPrintLobbyTxtBox();
	WaitingRoomInfo WRinfo;
	SOCKET sock;
	UserInfo *user;
	grideXY gride;
	vector<CHAT_LOG> ChatLog;

	int key;
	//UserInfo user;
	



};

