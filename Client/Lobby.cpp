#include "Lobby.h"
int Lobby::ClientMode = 0;
bool Lobby::threadOn = false;//�ӽ�
bool Lobby::ExitFlag = false;
bool Lobby::LogoutFlag = false;
HANDLE Lobby::hEventForRequest;

Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}
const int Lobby::LobbyMain(SOCKET sock)
{
	setSock(sock);
	td.sock = sock;
	td.lobby = this;
	hEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	hRcvThread = (HANDLE)_beginthreadex(NULL, 0, &RecvMsg, (void*)&td, 0, NULL);

	ClearXY();
	PrintLobbyListBox();
	GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
	PrintLobbyTxt();
	while (1)
	{
		key = getKeyDirectionCheck();
		Linfo.SetLobbyFlag(key);

		if (Linfo.GetLobbyFlag() == false)
		{

			Linfo.SetLobbyTxtNum(key);
			initRoomListCheck();
			AllClearPrintLobbyTxtBox();
			GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
			PrintLobbyTxt();
		}
		else
		{

			//RoomInfoListMain();
			Linfo.SetLobbyListPointNumber(key);
			initRoomListCheck();
			PrintLobbyListCheck(Linfo.GetLobbyListPointNumber());

			AllClearPrintLobbyTxtBox();
			PrintLobbyTxt();
		}



		if (Linfo.GetLobbyTxtNum() == 0) //�游���
		{
			if (key == SPACE || key == ENTER)
			{//[���ȣ]########�� ��
				if(req_CreateRoom())return 0;
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
		}
		//else if (Linfo.GetLobbyTxtNum() == 3)
		//{
		//	if (key == SPACE || key == ENTER)
		//	{
		//		//ClearXY();

		//		return 3; //��������(join)
		//	}
		//} �޴� ������ �� �������� ���� �����ϰ� �ҰŴϱ�!
		//�׷��� �긦 3���� �ؾ��� ���� �ȱ׷��� lobbyTxtNum �� 3�̾ȵ��ݾ�
		else if (Linfo.GetLobbyTxtNum() == 1) //logoout
		{
			if (key == SPACE || key == ENTER)
			{
				if (req_LogoutClient())
				{
					LogoutFlag = true;
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
		}
		else if (Linfo.GetLobbyTxtNum() == 2)// exit
		{
			if (key == SPACE || key == ENTER)
			{
				if (req_ExitClient())
				{
					ExitFlag = true;
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

void Lobby::setSock(SOCKET sock)
{
	this->sock = sock;
}
bool Lobby::req_CreateRoom()
{
	send(sock, "@R", 2, 0);
	cout << "�������û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest,3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_LogoutClient()
{
	send(sock, "@L", 2, 0);
	cout << "�α׾ƿ���û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_ExitClient()
{
	send(sock, "@G", 2, 0);
	cout << "���������û����" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
void Lobby::PrintWaitionRoomList(char *buf)
{//[�氳��]_[No.[���ȣ] ���̸�]_[���̸�]_[���̸�]
	//WaitingRoomCount
	char *tmp;
	int iRoomCount;
	initPrintWaitingRoomList();
	if (buf[0] == '0')
	{//���̾���
		Linfo.WaitingRoomCount = 0;
	}
	else
	{//���� �ִ°��

		tmp = strtok(buf, "_");
		iRoomCount = atoi(tmp);
		Linfo.WaitingRoomCount = iRoomCount;
		for (int i = 0; i < Linfo.WaitingRoomCount - 1; i++)
		{
			gotoxy(7 + 3, 5 + i);
			tmp = strtok(NULL, "_");
			cout << tmp << endl;
		}
	}


}

void Lobby::initPrintWaitingRoomList()
{
	for (int i = 0; i < 14; i++)
	{
		gotoxy(7 + 3, 5 + i);
		cout << "                    ";
	}
}

void Lobby::PrintLobbyListCheck(int WaitingRoomListPointNumber)
{
	gotoxy(7, 5 + WaitingRoomListPointNumber);
	cout << "�� ";
}


unsigned WINAPI Lobby::RecvMsg(void * arg)   // read thread main
{
	ThreadData td = *((ThreadData*)arg);
	char recvMsg[BUF_SIZE] = "";
	char *tmp_tok;
	int itmp_RoomNum;
	int strLen;
	//ClientMode >= 1(lobby����) �϶� �Ѿ���� �̺�Ʈ ó�� WaitSingleObject(hEvent,INFINITE);
	//�׷� �α׾ƿ��Ҷ� �����嵵 ���������߰���

	while (!ExitFlag && !LogoutFlag)
	{
		strLen = recv(td.sock, recvMsg, BUF_SIZE - 1, 0);	//�� �κп� send�����忡�� �����÷��װ� ������ �̺�Ʈ ó����....
		//���2. ��Ĺ�� �ͺ��ŷ���� ����� (ioctlsocket)
		if (strLen == -1)
			return -1;

		if (recvMsg[0] == '!')
		{
			//������ �޾ƿ�==> !"�氳��"_"No.[���ȣ]>> [���̸�]"_...
			//!0 �̸� ���̾��ٴ� ����
			//�ϴ� �������� ! ��� ������ ������ �Լ��� ����� �����ϴ��� üũ����
			if (recvMsg[1] == '0')td.lobby->PrintWaitionRoomList("0");
			else td.lobby->PrintWaitionRoomList(recvMsg + 1);
		}
		else if (recvMsg[0] == '/')
		{//ä�ø޽��� ����

		}
		else if (recvMsg[0] == '@')	//������Ʈ(req) �游���, ����(�α׾ƿ�)��û�Ϸ� �� �޽��� �޴°�
		{							//����� ���� ����(@R0, @R1), �����û�Ϸ�(@E1), �α׾ƿ�
			if (recvMsg[1] == 'R')
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(hEventForRequest);
				}
			}
			else if (recvMsg[1] == 'L')//�α׾ƿ���û�Ϸ�(@L1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(hEventForRequest);
					LogoutFlag = true;
				}
			}
			else if (recvMsg[1] == 'G')//�����û�Ϸ�(@E1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(hEventForRequest);
					ExitFlag = true;
				}
			}
		}
		else { cout << "�������� �޼���: " << recvMsg << endl; Sleep(100); }
	}
	cout << "exitRecvMsgThread" << endl;
	return 0;
}
