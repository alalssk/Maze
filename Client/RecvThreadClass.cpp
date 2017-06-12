#include "RecvThreadClass.h"

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

		if (recvMsg[0] == '!' && tData.user->IsCurrentClientMode(tData.user->GameState::LOBBY))
		{
			//������ �޾ƿ�==> !"�氳��"_"No.[���ȣ]>> [���̸�]"_...
			//!0 �̸� ���̾��ٴ� ����
			//�ϴ� �������� ! ��� ������ ������ �Լ��� ����� �����ϴ��� üũ����
			if (recvMsg[1] == '0')tData.lobby->GetWaitingRoomList("0");
			else tData.lobby->GetWaitingRoomList(recvMsg + 1);

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
				if (tData.user->wData.RoomNum == atoi(recvMsg + 3))// "$R_���ȣ" �� ���� �濡 ������ �ֵ��� �����÷��̸� �غ��϶�
				{
					tData.user->setClientMode(tData.user->GAMEPLAY);
					tData.wRoom->initWaitingRoom();
					// *** �� �ʱ�ȭ�� �����ָ� ���� �����Ҷ� ������ �ƴ� �ٸ� Ŭ�󿡼� WRinfo�� WaitingRoomTxtNum�� 2�λ��·� Ű�� ������ ������ �����û�� �����Ե�.
					tData.wRoom->PrintStartGameMsg();
				}
				
			}
			else if (recvMsg[1] == 'S' && 
				tData.user->IsCurrentClientMode(tData.user->GAMEPLAY))// $S���� ��Ŷ�� Ŭ���尡 GamePlay(3)�� ���¿��� ó���Ѵ�.
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
		}
		else if (recvMsg[0] == '@')	//������Ʈ(req) �游���, ����(�α׾ƿ�)��û�Ϸ� �� �޽��� �޴°�
		{							//����� ���� ����(@R0, @R1), �����û�Ϸ�(@E1), �α׾ƿ�
			if (recvMsg[1] == 'R')
			{
				if (recvMsg[2] == '1' && tData.user->IsCurrentClientMode(tData.user->LOBBY))
				{
					////////////////
					//��������ϱ� �������� �޾ƿ;߰���
					//@R1_[���ȣ]_[������]
					//�޴°� Ȯ��
					if (!tData.user->getRoomState())//���̾��°��(RoomState==false)
					{
						tData.user->setWaitingRoomData(recvMsg + 4); //[���ȣ]_[������]
						tData.user->setClientMode(tData.user->WAIT_ROOM);
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
					if (tData.user->IsCurrentClientMode(tData.user->WAIT_ROOM))//��������϶���
					{
						tData.user->setClientMode(tData.user->LOBBY);
						SetEvent(tData.wRoom->hWaitingRoomEventForRequest);
					}
				}
			}
			else if (recvMsg[1] == 'J')//�������û����
			{
				if (recvMsg[2] == '1')
				{
					if (tData.user->IsCurrentClientMode(tData.user->LOBBY))//�κ�����ϋ���
					{
						tData.user->setWaitingRoomData(recvMsg + 4);
						tData.user->setClientMode(tData.user->WAIT_ROOM);
						SetEvent(tData.lobby->hLobbyEventForRequest);
					}
				}
			}
			else if (recvMsg[1] == 'U')
			{//@U1_[�̸�]-�¼�_[�̸�-�¼�]
				if (recvMsg[2] == '1' && tData.user->IsCurrentClientMode(tData.user->WAIT_ROOM))
				{
					tData.user->setWaitingRoomUserList(recvMsg + 4);
					tData.user->setRoomUserKey();//RoomUserKey ����
					tData.wRoom->PrintUserList();
				}
			}
		}
		else if (recvMsg[0] == 'P' && tData.user->IsCurrentClientMode(tData.user->GAMEPLAY))
		{//P����Ű_����Ű
			tData.gPlay->RecvPlayerPosition(recvMsg + 1);
		}
		else if (recvMsg[0] == 'Q' && tData.user->IsCurrentClientMode(tData.user->GAMEPLAY))
		{//"Q����Ű" �̰� ����(Ŭ��� �ѹ���) ���� ���� ����
			int iUserKey;
			iUserKey = atoi(recvMsg + 1);
			tData.user->wData.Rating[iUserKey-1] = ++tData.user->wData.EndUserNum;
			if (tData.user->wData.EndUserNum == tData.user->wData.ConnectUserNum)
			{
				tData.user->setClientMode(tData.user->WAIT_ROOM);
				

			}
		}
		memset(recvMsg, 0, sizeof(recvMsg));

	}
	cout << "exitRecvMsgThread" << endl;
	return 0;
}