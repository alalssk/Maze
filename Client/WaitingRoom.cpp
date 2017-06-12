#include "WaitingRoom.h"
HANDLE WaitingRoom::hWaitingRoomEventForRequest;

WaitingRoom::WaitingRoom()
{
}


WaitingRoom::~WaitingRoom()
{
}
void WaitingRoom::AllClear()
{
	gride.ClearXY();
	gride.gotoxy(1, 1);
}
void WaitingRoom::PrintStartGameMsg()
{
	gride.DrawXY(12, 15, "+--------------------------------------------------+");
	gride.DrawXY(12, 16, "|           Press any key for start game           |");
	gride.DrawXY(12, 17, "+--------------------------------------------------+");
}
void WaitingRoom::initWaitingRoom()
{
	WRinfo.initWaitingRoomInfo();
}
void WaitingRoom::setUserInfo(UserInfo *input_user)
{
	user = input_user;
}
void WaitingRoom::req_SendMsgToServer(char* msg)
{//=================== "/[���ȣ]_[ID]_[����]
	char SendMsg[1024] = "";
	sprintf(SendMsg, "/%d_%s_%s", user->wData.RoomNum, user->getID(), msg);
	send(sock, SendMsg, strlen(SendMsg) + 1, 0);
}
int WaitingRoom::WatingRoomMain()
{
	hWaitingRoomEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	gride.ClearXY();
	Sleep(500);
	sock = user->getSocket();
	//user.setUserID("alalssk");/*###�ӽ� ä�÷α� �������*/
	gride.DrawXY(5, 3, " No. ");
	gride.gotoxy(5 + 5, 3); cout << user->wData.RoomNum;
	gride.DrawXY(5 + 15, 3, "Name: ");
	gride.gotoxy(5 + 15 + 6, 3); cout << user->wData.RoomName;
	gride.GrideBox(5, 4, 3, 18);//PrintUserListBox
	PrintUserList();
	gride.GrideBox(5, 9, 20, 18);//PrintChatBox
	//PrintChat()
	PrintButton();
	char inputstr[MAXCHAR];
	int inputstrSz = 0;
	memset(inputstr, 0, sizeof(inputstr));

	while (!(user->IsCurrentClientMode(GameState::GAMEPLAY)))//(user->getClientMode() != 3))
	{
		key = gride.getKeyDirectionCheck();
		WRinfo.SetWaitingRoomFlag(key);//���� ������ ����(�޴�����, ä��)
		if (WRinfo.GetWaitingRoomFlag() == false)
		{//�޴����úκ�
			if (key == ENTER || key == SPACE)
			{	//����κ� �游 �����°ɷ� ��������� �κ�� �α��� ȭ�鿡����
				//@E_[���ȣ]_[ID]
				if (WRinfo.GetWaitingRoomTxtNum() == 2)//exit room
				{
					sprintf(inputstr, "@E_%d_%s", user->wData.RoomNum, user->getID());
					send(sock, inputstr, strlen(inputstr) + 1, 0);
					memset(inputstr, 0, sizeof(inputstr)); inputstrSz = 0;
					if (WaitForSingleObject(hWaitingRoomEventForRequest, 5000) == WAIT_OBJECT_0)
					{
						//������ ���� >> �� �����°Ŵϱ� �ƾ� ������ �ʱ�ȭ���ѹ���
						user->ExitWaitingRoom();
						break;
					}
					else
					{
						continue;
					}
				}
				else if (WRinfo.GetWaitingRoomTxtNum() == 1)//���ӽ��۹�ư
				{
					if (strcmp(user->wData.UserName[0], user->getID()) == 0) // ���常 ������ ����
					{
						char StartMsg[3 + 4 + 13] = "";
						sprintf(StartMsg, "$R_%d", user->wData.RoomNum);//$R_���ȣ ���� >> �ش� �� ���ӽ��� ��û

						send(sock, StartMsg, strlen(StartMsg) + 1, 0);

						key = gride.getKeyDirectionCheck();//���常 break�� �����ϸ� �ٸ� ������ �ƴѾֵ��̶� �ٸ����°� �Ǵϱ� 
					}
					else {
						//���常 �����Ҽ� �־�� ���
					}
				}
			}
			else
			{
				WRinfo.SetWaitingRoomTxtNum(key);
				PrintButton();
			}
		}
		else
		{//ä�úκ� ����
			if (key == ENTER)
			{//���������� ��ĥ�κ�
				//
				//ChattingSendToServer(string chat) >> �� �Լ� �ȿ� InputChatLog("alalssk", inputstr); ���Լ��� ����
				//���⼱ ä���� ������ �����⸸�ϰ� inputstr�̶� sz�ʱ�ȭ���ش�
				//ä�� ��°�  ä��â �ʱ�ȭ�� Recv�����忡�� �Ұ���
				//=================================
				req_SendMsgToServer(inputstr);
				//InputChatLog(user->getID(), inputstr);
				memset(inputstr, 0, sizeof(inputstr)); inputstrSz = 0;
				//initChatListBox();//ä��â �ʱ�ȭ
				//PrintChatLogList();//���
				gride.DrawXY(7, 28, "                                   "); 
				gride.gotoxy(7, 28);
			}
			else
			{
				//initChatListBox();
				if (key != LEFT &&key != RIGHT && key != UP && key != DOWN &&key != ENTER){ inputstr[inputstrSz++] = key; }
				gride.DrawXY(7, 28, inputstr);
			}
			//ä�ù� ������ ä����¿� ������ ���� ä��list�� �Է���������(ŸŬ��κ���(recv) ä�����
		}
	}//end while
	return 0;
}

void WaitingRoom::PrintUserList()
{
	Sleep(500);
	for (int i = 0; i < MAX_USERNUM; i++)
	{//�ӽ� �ʱ�ȭ
		gride.DrawXY(7, 5 + i, "                                     ");
	}
	for (int i = 0; i < MAX_USERNUM; i++)//MAX user 3
	{
		if (i < user->wData.ConnectUserNum)
		{
			gride.DrawXY(7, 5 + i, user->wData.UserName[i]);
		}
		else {
			gride.DrawXY(7, 5 + i, "                              ");
		}
	}
}
void WaitingRoom::PrintButton()
{
	AllClearPrintLobbyTxtBox();
	gride.GrideBox(50, 4 * WRinfo.GetWaitingRoomTxtNum(), 1, 7);// 4 * ButtonNumber(1,2)
	gride.DrawXY(56, 5, "Start !");//or Ready !!
	gride.DrawXY(56, 9, "Exit !!");
}
void WaitingRoom::AllClearPrintLobbyTxtBox()
{
	for (int i = 0; i < 8; i++)
	{
		gride.DrawXY(50, 4 + i, "                    ");
	}
}
bool WaitingRoom::InputChatLog(string name, string chat)
{
	ChatLog.push_back(pair<string, string>(name, chat));
	return true;//����ó�� �߰��ϼ�
}
void WaitingRoom::PrintChatLogList()
{
	//7,28 ���� y�� --
	for (int i = 1; i <= 17; i++)
	{
		if (ChatLog.size() < i)break;
		gride.gotoxy(7, 27 - i);
		cout << ChatLog[ChatLog.size() - i].first << ": " << ChatLog[ChatLog.size() - i].second;
	}
}
void WaitingRoom::initChatListBox()
{
	for (int i = 0; i < 18; i++)
	{
		gride.DrawXY(7, 11 + i, "                                    ");
	}
}