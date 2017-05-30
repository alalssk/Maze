#include "WaitingRoom.h"
HANDLE WaitingRoom::hWaitingRoomEventForRequest;

WaitingRoom::WaitingRoom()
{
}


WaitingRoom::~WaitingRoom()
{
}
void WaitingRoom::setUserInfo(UserInfo *input_user)
{
	user = input_user;
}
int WaitingRoom::WatingRoomMain()
{
	hWaitingRoomEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClearXY();
	Sleep(500);
	sock = user->getSocket();
	//user.setUserID("alalssk");/*###�ӽ� ä�÷α� �������*/
	gotoxy(5, 3);	cout << " No. " << 1234567;//gotoxy(5+5,3); cout<<RoomNum;
	gotoxy(5 + 15, 3); cout << "Name: " << " ����������������";
	GrideBox(5, 4, 3, 18);//PrintUserListBox
	PrintUserList();
	GrideBox(5, 9, 20, 18);//PrintChatBox
	//PrintChat()
	PrintButton();
	char inputstr[MAXCHAR];
	int inputstrSz = 0;
	memset(inputstr, 0, sizeof(inputstr));
	while (1)
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
					send(sock, inputstr, strlen(inputstr)+1, 0);
					memset(inputstr, 0, sizeof(inputstr)); inputstrSz = 0;
					if (WaitForSingleObject(hWaitingRoomEventForRequest, 5000) == WAIT_TIMEOUT)continue;
					else
					{
						//������ ���� >> �� �����°Ŵϱ� �ƾ� ������ �ʱ�ȭ���ѹ���
						user->ExitWaitingRoom();
						break;
					}

				}
				else if (WRinfo.GetWaitingRoomTxtNum() == 1)//���ӽ��۹�ư - ���常 ������ �ְ�
				{

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

				InputChatLog(user->getID(), inputstr);
				memset(inputstr, 0, sizeof(inputstr)); inputstrSz = 0;
				initChatListBox();//ä��â �ʱ�ȭ
				PrintChatLogList();//���
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
		if (i < user->wData.ConnectUserNum-1)
		{
			gotoxy(7, 5 + i); cout << user->wData.UserName[i];

		}
		else {
			gotoxy(7, 5 + i); cout << "          �ʱ�ȭ                ";
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