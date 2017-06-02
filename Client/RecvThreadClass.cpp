#include "RecvThreadClass.h"

int RecvThreadClass::ClientMode = 0;//�α���ȭ��(0), �κ�(1), ����(2), ������(3);
bool RecvThreadClass::threadOn = false;//�ӽ�
bool RecvThreadClass::ExitFlag = false;
bool RecvThreadClass::LogoutFlag = false;
RecvThreadClass::RecvThreadClass()
{
}


RecvThreadClass::~RecvThreadClass()
{
}
void RecvThreadClass::StartThread()
{

	if (!threadOn)
	{
		hRcvThread = (HANDLE)_beginthreadex(NULL, 0, &RecvMsg, (void*)&tData, 0, NULL);
		threadOn = true;

	}

}
void RecvThreadClass::setUserInfo(UserInfo* input_user)
{
	tData.user = input_user;
}
unsigned WINAPI RecvThreadClass::RecvMsg(void * arg)   // read thread main
{
	ThreadData tData = *((ThreadData*)arg);
	char recvMsg[BUF_SIZE] = "";
	char recvID[13] = "";
	char *tmp_tok;
	int itmp_RoomNum;
	int strLen;
	//ClientMode >= 1(lobby����) �϶� �Ѿ���� �̺�Ʈ ó�� WaitSingleObject(hEvent,INFINITE);
	//�׷� �α׾ƿ��Ҷ� �����嵵 ���������߰���

	while (!ExitFlag && !LogoutFlag)
	{
		strLen = recv(tData.sock, recvMsg, BUF_SIZE - 1, 0);
		//�� �κп� send�����忡�� �����÷��װ� ������ �̺�Ʈ ó����....
		//���2. ��Ĺ�� �ͺ��ŷ���� ����� (ioctlsocket)
		if (strLen == -1)
			return -1;

		if (recvMsg[0] == '!' && ClientMode == 1)
		{
			//������ �޾ƿ�==> !"�氳��"_"No.[���ȣ]>> [���̸�]"_...
			//!0 �̸� ���̾��ٴ� ����
			//�ϴ� �������� ! ��� ������ ������ �Լ��� ����� �����ϴ��� üũ����
			if (recvMsg[1] == '0')tData.lobby->GetWaitionRoomList("0");
			else tData.lobby->GetWaitionRoomList(recvMsg + 1);

			memset(recvMsg, 0, sizeof(recvMsg));
		}
		else if (recvMsg[0] == '/')
		{	//ä�ø޽��� ����
			//���ȣ�� ���� �����ʿ���� �濡�ִ¾ֵ����׸� �޽����� �����ϱ�
			//"/1_[������ID]_[����]"
			tmp_tok = strtok(recvMsg, "_");
			tmp_tok = strtok(NULL, "_");
			strcpy(recvID, tmp_tok);
			tmp_tok = strtok(NULL, "_");
			tData.wRoom->InputChatLog(recvID, tmp_tok);
			tData.wRoom->initChatListBox();//ä��â �ʱ�ȭ
			tData.wRoom->PrintChatLogList();//���

		}
		else if (recvMsg[0] == '$')
		{//============ ������ ������ ���� "$R_���ȣ" ���� ���� >> �̰� ��� Ŭ�� �Ѹ��� ��Ŷ��
			if (recvMsg[1] == 'R')
			{
				if (tData.user->wData.RoomNum == atoi(recvMsg + 3))// "$R_���ȣ" �� �濡 ������ �ֵ��� �غ��϶�
				{
					ClientMode = 3; tData.user->setClientMode(3);
					tData.wRoom->PrintStartGameMsg();
				}
				
			}
			else if (recvMsg[1] == 'S' && ClientMode ==3)// $S���� ��Ŷ�� Ŭ���尡 GamePlay(3)�� ���¿��� ó���Ѵ�.
			{//$S1_[ID] �ִ� ���̵� �����غ����
				if (recvMsg[2] == '1')
				{
					for (int i = 0; i < tData.user->wData.ConnectUserNum; i++)
					{
						if (strcmp(tData.user->wData.UserName[i], recvMsg + 4) == 0)
						{
							tData.user->wData.UserState[i] = true;
						}
					}
				}

			}
			
			//char input[100]="";
			//
			//for (int i = 0; i < 5; i++)
			//{
			//	cin >> input;
			//	cout << "input : "<<input<<endl;
			//}
			//tData.wRoom->WatingRoomMain();


		}
		else if (recvMsg[0] == '@')	//������Ʈ(req) �游���, ����(�α׾ƿ�)��û�Ϸ� �� �޽��� �޴°�
		{							//����� ���� ����(@R0, @R1), �����û�Ϸ�(@E1), �α׾ƿ�
			if (recvMsg[1] == 'R')
			{
				if (recvMsg[2] == '1' && ClientMode == 1)
				{
					////////////////
					//��������ϱ� �������� �޾ƿ;߰���
					//@R1_[���ȣ]_[������]
					//�޴°� Ȯ��
					if (!tData.user->getRoomState())//���̾��°��(RoomState==false)
					{
						tData.user->setWaitingRoomData(recvMsg + 4); //[���ȣ]_[������]
						ClientMode = 2; tData.user->setClientMode(2);
						SetEvent(tData.lobby->hLobbyEventForRequest);
					}
					else
					{
						cout << "[����� ����] �̹� �濡 �������Դϴ�" << endl;
					}

				}
			}
			else if (recvMsg[1] == 'r')//���û(���۽�)
			{
				if (recvMsg[2] == '1')
				{

					SetEvent(tData.lobby->hLobbyEventForRequest);
				}
			}
			else if (recvMsg[1] == 'L')//�α׾ƿ���û�Ϸ�(@L1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(tData.lobby->hLobbyEventForRequest);
					LogoutFlag = true;
				}
			}
			else if (recvMsg[1] == 'G')//�����û�Ϸ�(@G1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(tData.lobby->hLobbyEventForRequest);
					ExitFlag = true;
				}
			}
			else if (recvMsg[1] == 'E')//�泪�����û �Ϸ�
			{
				if (recvMsg[2] == '1')
				{
					if (ClientMode == 2)//��������϶���
					{
						ClientMode = 1; tData.user->setClientMode(1);
						SetEvent(tData.wRoom->hWaitingRoomEventForRequest);
					}
				}
			}
			else if (recvMsg[1] == 'J')//�������û����
			{
				if (recvMsg[2] == '1')
				{
					if (ClientMode == 1)//�κ�����ϋ���
					{
						tData.user->setWaitingRoomData(recvMsg + 4);
						ClientMode = 2; tData.user->setClientMode(2);
						SetEvent(tData.lobby->hLobbyEventForRequest);
					}
				}
			}
			else if (recvMsg[1] == 'U')
			{//@U1_[�̸�]-�¼�_[�̸�-�¼�]
				if (recvMsg[2] == '1' && ClientMode == 2)
				{
					tData.user->setWaitingRoomUserList(recvMsg + 4);
					tData.wRoom->PrintUserList();
				}
			}
		}
		else {}
		memset(recvMsg, 0, sizeof(recvMsg));

	}
	cout << "exitRecvMsgThread" << endl;
	return 0;
}