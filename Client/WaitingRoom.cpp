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
	ClearXY();
	gotoxy(1, 1);
}
void WaitingRoom::PrintStartGameMsg()
{
	gotoxy(12, 15); cout << "+--------------------------------------------------+";
	gotoxy(12, 16); cout << "|           Press any key for start game           |";
	gotoxy(12, 17); cout << "+--------------------------------------------------+";
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

	//switch (WaitForSingleObject(hWaitingRoomEventForRequest, 5000))
	//{
	//case WAIT_TIMEOUT:
	//	return false; break;
	//case WAIT_OBJECT_0:
	//	return true; break;
	//default:
	//	return false; break;
	//}
}
int WaitingRoom::WatingRoomMain()
{
	hWaitingRoomEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClearXY();
	Sleep(500);
	sock = user->getSocket();
	//user.setUserID("alalssk");/*###�ӽ� ä�÷α� �������*/
	gotoxy(5, 3);	cout << " No. "; gotoxy(5 + 5, 3); cout << user->wData.RoomNum;
	gotoxy(5 + 15, 3); cout << "Name: "; gotoxy(5 + 15 + 6, 3); cout << user->wData.RoomName;
	GrideBox(5, 4, 3, 18);//PrintUserListBox
	PrintUserList();
	GrideBox(5, 9, 20, 18);//PrintChatBox
	//PrintChat()
	PrintButton();
	char inputstr[MAXCHAR];
	int inputstrSz = 0;
	memset(inputstr, 0, sizeof(inputstr));
	while ((user->getClientMode() != 3))
	{
		key = getKeyDirectionCheck();
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
				gotoxy(7, 28); cout << "                                   ";
				gotoxy(7, 28);
			}
			else
			{
				//initChatListBox();
				if (key != LEFT &&key != RIGHT && key != UP && key != DOWN &&key != ENTER)inputstr[inputstrSz++] = key;
				gotoxy(7, 28); 	cout << inputstr;
			}


			//ä�ù� ������ ä����¿� ������ ���� ä��list�� �Է���������(ŸŬ��κ���(recv) ä�����


		}
	}
	return 0;
}

void WaitingRoom::PrintUserList()
{
	Sleep(500);
	for (int i = 0; i < 3; i++)
	{//�ӽ� �ʱ�ȭ
		gotoxy(7, 5 + i); cout << "                                         ";
	}
	for (int i = 0; i < 3; i++)//MAX user 3
	{
		if (i < user->wData.ConnectUserNum)
		{
			gotoxy(7, 5 + i); cout << user->wData.UserName[i];

		}
		else {
			gotoxy(7, 5 + i); cout << "          �ʱ�ȭ              ";
		}


		//	cout << "READY!!" << "  ||  " << "DIDIDIDIDIDID" << "  ||  " << 15 << endl;//state || ID || WinCount
	}
}
void WaitingRoom::PrintButton()
{
	AllClearPrintLobbyTxtBox();
	GrideBox(50, 4 * WRinfo.GetWaitingRoomTxtNum(), 1, 7);// 4 * ButtonNumber(1,2)
	gotoxy(56, 5); cout << "Start !";//or Ready !!
	gotoxy(56, 9); cout << "Exit !!";
}
void WaitingRoom::AllClearPrintLobbyTxtBox()
{
	for (int i = 0; i < 8; i++)
	{
		gotoxy(50, 4 + i);
		cout << "                    ";
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
		gotoxy(7, 27 - i);
		cout << ChatLog[ChatLog.size() - i].first << ": " << ChatLog[ChatLog.size() - i].second;
	}

}
void WaitingRoom::initChatListBox()
{
	for (int i = 0; i < 18; i++)
	{
		gotoxy(7, 11 + i);
		cout << "                                    ";
	}
}