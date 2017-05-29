#include "Lobby.h"

HANDLE Lobby::hLobbyEventForRequest;

Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}
const int Lobby::LobbyMain()
{
	sock = user->getSocket();
	hLobbyEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClearXY();
	PrintLobbyListBox();
	GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
	Sleep(50);
	req_GetWaitingRoom();
	Sleep(50);
	PrintWaitionRoomList();
	PrintLobbyTxt();
	while (1)
	{
		key = getKeyDirectionCheck();
		Linfo.SetLobbyFlag(key);
		
		if (Linfo.GetLobbyFlag() == false)
		{
			if (key == SPACE || key == ENTER)
			{//�޴�����Ȯ��
				if (Linfo.GetLobbyTxtNum() == 0) //�游���
				{
					if (req_CreateRoom())return 0;
					else
					{
						for (int i = 0; i < 3; i++)
						{
							gotoxy(40, 15);
							cout << "###########����� ����#############";
							Sleep(500);
							gotoxy(40, 15);
							cout << "                                   ";
							Sleep(500);
						}
						continue;
					}
				}
				else if (Linfo.GetLobbyTxtNum() == 1) //logoout
				{
					if (req_LogoutClient())
					{

						return 4;

					}
					else
					{
						for (int i = 0; i < 3; i++)
						{
							gotoxy(40, 15);
							cout << "###########�α׾ƿ� ����#############";
							Sleep(500);
							gotoxy(40, 15);
							cout << "                                     ";
							Sleep(500);
						}
						continue;
					}
				}
				else if (Linfo.GetLobbyTxtNum() == 2)// exit
				{
					if (req_ExitClient())
					{

						return 2;

					}
					else
					{
						for (int i = 0; i < 3; i++)
						{
							gotoxy(40, 15);
							cout << "###########�������� ����#############";
							Sleep(500);
							gotoxy(40, 15);
							cout << "                                     ";
							Sleep(500);
						}
						continue;
					}
				}

			}
			else
			{//�޴�����(����Ű)
				Linfo.SetLobbyTxtNum(key);
				initRoomListCheck();
				AllClearPrintLobbyTxtBox();
				GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
				//PrintWaitionRoomList();
				PrintLobbyTxt();
			}
		}
		else
		{//�漱�� �κ�

			//RoomInfoListMain();
			if (key == SPACE || key == ENTER)
			{	//WaitingRoomList[LobbyListPointNum] -> ���� ���̸�
				//send�� ���ȣ�� send�ؾ��Ѵ�
				//Linfo.WaitingRoomList[]���� "No.2>>[���̸�]" �̷������� �������� �� ��ȣ�� ���� send
				char * tmp;
				int iRoomNum;
				tmp = strtok(Linfo.WaitingRoomList[Linfo.GetLobbyListPointNumber()], ".");
				tmp = strtok(NULL, ">");
				iRoomNum = atoi(tmp);
				if (req_EnterWaitingRoom(iRoomNum))//�����强��
				{
					
					return 3;
				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						gotoxy(40, 15);
						cout << "###########������ ����#############";
						Sleep(500);
						gotoxy(40, 15);
						cout << "                                     ";
						Sleep(500);
					}
					continue;
				}
				//Linfo.WaitingRoomList[Linfo.GetLobbyListPointNumber()];
			}
			else
			{
				Linfo.SetLobbyListPointNumber(key);
				initRoomListCheck();
				PrintWaitionRoomList();
				PrintLobbyListCheck(Linfo.GetLobbyListPointNumber());

				AllClearPrintLobbyTxtBox();
				PrintLobbyTxt();
			}

			

		}




	}

}
void Lobby::PrintLobbyListBox()
{
	int i = 0;
	gotoxy(16, 3);
	cout << " �� �� �� �� ��" << endl;
	GrideBox(5, 4, 14, 18);
}
void Lobby::AllClearPrintLobbyTxtBox()
{
	for (int i = 0; i < 12; i++)
	{
		gotoxy(46, 6 + i);
		cout << "                ";
	}
}
void Lobby::PrintLobbyTxt()
{
	for (int i = 0; i < LOBBY_TXT_NUM_MAX; i++)
	{
		gotoxy(48, 7 + i * 3);
		switch (i)
		{
		case 0:
			cout << "�� �� �� ��"; break;
		case 1:
			cout << "�� �� �� ��"; break;
		case 2:
			cout << "�� �� �� ��"; break;
		}
	}
}

void Lobby::initRoomListCheck()
{
	for (int i = 0; i < 14; i++)
	{
		gotoxy(7, 5 + i);
		cout << "   ";
	}

}
void Lobby::setUserInfo(UserInfo *user)
{
	this->user = user;
}
void Lobby::setSock(SOCKET sock)
{
	this->sock = sock;
}
bool Lobby::req_GetWaitingRoom()//���û(���۽�)
{
	send(sock, "@r", 2, 0);
	DWORD ret;
	ret = WaitForSingleObject(hLobbyEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_CreateRoom()
{
	send(sock, "@R", 2, 0);
	//cout << "�������û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hLobbyEventForRequest,3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_LogoutClient()
{
	send(sock, "@L", 2, 0);
	//cout << "�α׾ƿ���û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hLobbyEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_ExitClient()
{
	send(sock, "@G", 2, 0);
	//cout << "���������û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hLobbyEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_EnterWaitingRoom(int RoomNum)
{
	char sendstr[7];//@G_9999
	sprintf(sendstr, "@J_%d", RoomNum);
	send(sock, sendstr, 7, 0);
	//cout << "���������û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hLobbyEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
void Lobby::PrintWaitionRoomList()
{
	initPrintWaitingRoomList();
	gotoxy(0, 0); Sleep(100);
	for (int i = 0; i < Linfo.WaitingRoomListNum; i++)
	{
		gotoxy(7 + 3, 5 + i); cout << Linfo.WaitingRoomList[i];

	}
}
void Lobby::GetWaitionRoomList(char *buf)
{//[�氳��]_[No.[���ȣ] ���̸�]_[���̸�]_[���̸�]
	//WaitingRoomCount
	char *tmp;
	int iRoomCount;
	
	memset(Linfo.WaitingRoomList, 0, sizeof(Linfo.WaitingRoomList));//2�����迭(tmp[max][max]) �ʱ�ȭ�ϴ� �� Ȯ���ϰ� �׽�Ʈ ����
																	//memset(tmp,0,sizeof(tmp)) �ص� 2���� �� �ʱ�ȭ �ȴ�.
	if (buf[0] == '0')
	{//���̾���
		Linfo.WaitingRoomListNum = 0;
	}
	else
	{//���� �ִ°��

		tmp = strtok(buf, "_");
		iRoomCount = atoi(tmp);
		Linfo.WaitingRoomListNum = iRoomCount;
		for (int i = 0; i < Linfo.WaitingRoomListNum; i++)
		{
			tmp = strtok(NULL, "_");
			strcpy(Linfo.WaitingRoomList[i], tmp);
			
		}
	}
	Linfo.initLobbyListPointNumber();
	PrintWaitionRoomList();

}

void Lobby::initPrintWaitingRoomList()
{
	for (int i = 0; i < 14; i++)
	{
		gotoxy(7 + 3, 5 + i);
		cout << "                              ";
	}
}

void Lobby::PrintLobbyListCheck(int WaitingRoomListPointNumber)
{
	gotoxy(7, 5 + WaitingRoomListPointNumber);
	cout << "�� ";
}


