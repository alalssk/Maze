#include "ServerClass.h"
int ServerClass::TotalConnectedClientCount = 0;
int ServerClass::TotalCreateRoomCount = 0;
bool ServerClass::ExitFlag = false;
int ServerClass::ChatRoomCount = 0;
LogClass ServerClass::Chatlog;
LogClass ServerClass::DBLog;
ServerDB ServerClass::sDB;
CRITICAL_SECTION ServerClass::cs;
ServerClass::ServerClass()
{
	shareData = new Shared_DATA;
	//memset(lpComPort, 0, sizeof(ComPort)); �� �̷��� �ʱ�ȭ�ϸ� push_back �κп��� ũ������ ������...?
	shareData->flags = 0;
	shareData->Clients_Num = 0;

}


ServerClass::~ServerClass()
{
	delete shareData;
	cout << "Delete CompletionPort" << endl;
}
void ServerClass::printConnectClientNum()
{
	cout << TotalConnectedClientCount << "�� ������" << endl;
}
bool ServerClass::ServerClassMain()
{
	Chatlog.OpenFile("ChatLog.txt");
	DBLog.OpenFile("DBLog.txt");
	//start DB
	if (!sDB.StartDB())
	{
		return false;
	}
	//start socket
	shareData->hServSock = GetListenSock(9191, SOMAXCONN);
	if (shareData->hServSock == INVALID_SOCKET)
	{
		cout << "GetListenSockError(" << GetLastError() << ')' << endl;
		return false;
	}
	else{
		cout << "GetListenSock() is ok" << endl;
	}

	if (!Create_IOCP_ThreadPool())
	{
		cout << "Create ThreadPool for IOCP is failed..." << endl;

		return false;
	}
	else{
		cout << "Create ThreadPool for IOCP is Ok ..." << endl;
		cout << "Start AcceptThread." << endl;
	}
	InitializeCriticalSection(&cs);
	hTheards[0] = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, (void*)shareData, 0, NULL);//Begin Accept Thread
	cout << "servermain ok" << endl;

	return true;
}
unsigned ServerClass::AcceptThread(PVOID pComPort)
{
	LPShared_DATA lpComPort = (LPShared_DATA)pComPort;
	CLIENT_DATA client_data;

	int addrLen = sizeof(client_data.clntAdr);

	LPOVER_DATA ioInfo;

	while (!ExitFlag)
	{
		printf("%d�� ������....", lpComPort->Clients_Num);
		printf("%d �� ° Ŭ���̾�Ʈ Accept �����\n", TotalConnectedClientCount);
		client_data.hClntSock = accept(
			lpComPort->hServSock,
			(SOCKADDR*)&client_data.clntAdr,
			&addrLen
			);

		cout << "Accept ó����..." << endl;
		//
		if (CreateIoCompletionPort(
			(HANDLE)client_data.hClntSock,
			lpComPort->hComPort,
			(DWORD)client_data.hClntSock,//���ø���Ű�� 
			0
			) == NULL)
		{
			cout << "Client socket connect to IOCP handle error : " << GetLastError() << endl;
			continue;
		}

		ioInfo = new OVER_DATA;
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->Mode = FIRST_READ;//ù ���� ó���� ���� ���(ID password ó��)
		/*IO_PENDING ����ó��*/
		cout << "client sock: " << client_data.hClntSock << '-' << inet_ntoa(client_data.clntAdr.sin_addr) << endl;
		if (WSARecv(
			client_data.hClntSock,
			&(ioInfo->wsaBuf),
			1,
			&lpComPort->recvBytes,
			&lpComPort->flags,
			&(ioInfo->overlapped),
			NULL
			) == SOCKET_ERROR)
		{
			switch (WSAGetLastError())
			{
			case WSA_IO_PENDING:
				cout << "An overlapped operation was successfully initiated and completion will be indicated at a later time." << endl; break;
			default:
				cout << "WSAGetLastError code(" << WSAGetLastError() << ')' << endl;
				break;
			}
		}



	}


	return 0;
}
bool ServerClass::Create_IOCP_ThreadPool()
{

	shareData->hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (shareData->hComPort == INVALID_HANDLE_VALUE)
	{
		cout << "CreateIoCompletionPort Error(" << GetLastError() << ')' << endl;
		return false;
	}
	GetSystemInfo(&sysInfo);
	for (int i = 0; i < (int)sysInfo.dwNumberOfProcessors; i++)
		_beginthreadex(NULL, 0, &IOCPWorkerThread, (LPVOID)shareData, 0, NULL);

	return true;

}
unsigned  __stdcall ServerClass::IOCPWorkerThread(LPVOID CompletionPortIO)
{
	//HANDLE hComPort = (HANDLE)pComPort;
	LPShared_DATA shareData = (LPShared_DATA)CompletionPortIO;

	/*���ø���Ű*/
	SOCKET sock;
	/*���ø���Ű*/

	DWORD bytesTrans;
	LPOVER_DATA ioInfo;
	DWORD flags = 0;
	char SendMsg[1024];
	char clntName[MAX_NAME_SIZE];
	memset(SendMsg, 0, (sizeof(SendMsg)));
	memset(clntName, 0, (sizeof(clntName)));
	CLIENT_DATA client_data;

	//list<CLIENT_DATA>::iterator iter;
	list<ChatRoom>::iterator iter_ChatRoom;
	while (1)
	{
		BOOL bGQCS = GetQueuedCompletionStatus(
			shareData->hComPort,
			&bytesTrans,
			&sock,//���ø���Ű
			(LPOVERLAPPED*)&ioInfo,
			INFINITE
			);//error���� �� ������� 
		if (bGQCS)//gqcs ����
		{

			if (ioInfo->Mode == FIRST_READ)//ID_PASS �ԷµȰ� DBó��
			{
				char first_send[5] = "";
				int DBcode;
				if (sDB.Check_Password(ioInfo->buffer))
				{
					DBcode = 0;//���Ӥ���
					EnterCriticalSection(&cs);

					client_data.hClntSock = sock;
					strcpy(client_data.name, strtok(ioInfo->buffer, "_"));
					client_data.MyRoom = 0;
					shareData->Clients.push_back(client_data);//list
					shareData->Clients_Num++;
					cout << '[' << client_data.name << ']' << client_data.hClntSock << "���� ������ - " << endl;
					TotalConnectedClientCount++;
					LeaveCriticalSection(&cs);
				}
				else {
					DBcode = 2;//����ٸ��ڵ�;
					strcpy(client_data.name, strtok(ioInfo->buffer, "_"));//�긦 cs�� ���� �ʿ䰡 �ִ���...?
					cout << '[' << client_data.name << "]���Ʋ��" << endl;

				}
				//

				//

				//shareData->Clients.push_back()
				sprintf(first_send, "@%d", DBcode);
				//SendMsgFunc(first_send, shareData, 5);
				//�翬�� �ش� ��Ĺ���� �����ߵǴ��� ��üsend(SnedMsgFunc)�� �ذ����� ����
				send(sock, first_send, 5, 0);
				delete ioInfo;
				//����� ioInfo->Mode = ROOM_READ �� �����ϰ� �� ������ recv send ��
				//Ư�� �ڵ�(����� ������ �����ڵ� ��)�� ������ �׿����� ��� ó��

				/*WSARecv*/
				ioInfo = new OVER_DATA;
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				if (DBcode == 2)ioInfo->Mode = FIRST_READ; //����ٸ��� �ٽ� �о���ϴϱ�
				else ioInfo->Mode = READ;
			}
			else if (ioInfo->Mode == READ)
			{
				if (ioInfo->buffer[0] == '@')//�����(@R), ������(@J)-MyRoom�� 0�� ��츸, �� ������(@E)-MyRoom�� 0�� �ƴ� ��츸
				{
					if (ioInfo->buffer[1] == 'R')					//****** �� ���� ��û ******
					{												//�� ���� �Ϸ� �� ��� Ŭ���̾�Ʈ�� ���ο� ��鿡 ���� ������ send��
						if (CreateRoomFunc(shareData, sock))//�� ����
						{//�� �����Ҷ��� ���������
							//�׸��� ���Ŭ�� ���ο� ������(!"�氳��"_"No.[���ȣ]>> [���̸�]"_... send

							//cout << "�� �����Ϸ�" << endl;
							//send(sock, "@R1", 3, 0); -----> CreateRoomFunc �Լ����ο��� �Ϸ���Ŷ����
							SendWaitingRoomList(shareData);
						}
						else
						{
							cout << "�� ���� ���� -> " << sock << endl;
							send(sock, "@R0", 3, 0);
						}


					}
					else if (ioInfo->buffer[1] == 'r')				//****** �渮��Ʈ ��û ******
					{
						send(sock, "@r1", 3, 0);
						SendWaitingRoomList(shareData);
					}
					else if (ioInfo->buffer[1] == 'J')				//****** �� ���� ��û Join ******
					{//@J_[���ȣ]
						char *cRoomNum;
						int iRoomNum;
						strtok(ioInfo->buffer, "_");
						cRoomNum = strtok(NULL, "_");
						iRoomNum = atoi(cRoomNum);
						cout << "�� ���� ��Ŷ ����" << iRoomNum << endl;
						if (JoinRoomFunc(shareData, sock, iRoomNum))
						{//�����强��
							//������ ���� JoinRoomFunc���� ������Ŷ send��
							cout << "�����强��(@J1) ����->" << sock << endl;
						}
						else
						{//������ ����
							send(sock, "@J0", 3, 0);
							cout << "���������(@J0) ����->" << sock << endl;
						}
						//func(shareData, sock, roomNum)
					}
					else if (ioInfo->buffer[1] == 'E')				//****** �� ������ ��û Exit 
					{//@E_[���ȣ]_[ID]
						cout << "�� ������ ��û ��Ŷ ����" << endl;
						char *tmp;

						int iRoomNum;
						strtok(ioInfo->buffer, "_");
						tmp = strtok(NULL, "_");
						iRoomNum = atoi(tmp);
						tmp = strtok(NULL, "_");

						if (ExitRoomFunc(shareData, iRoomNum, tmp))
						{
							send(sock, "@E1", 3, 0);

							cout << "�� ������ ��û �Ϸ���Ŷ(@E1) ����" << endl;
						}
						else
						{
							send(sock, "@E0", 3, 0);

							cout << "�� ������ ��û ������Ŷ(@E0) ����" << endl;
						}


					}
					else if (ioInfo->buffer[1] == 'L')				//****** ���� �α׾ƿ���û ******
					{//
						cout << "�α׾ƿ� ��û ��Ŷ ���� -> " << sock;
						/*�α׾ƿ� ó���κ� ����� �α׾ƿ��� �������Ḧ ���� ���ָ� ������ �ϴ� ������ ����*/

						send(sock, "@L1", 3, 0);
						cout << "--->(@L1)���� �Ϸ�" << endl;
						CloseClientSock(sock, ioInfo, shareData);

					}
					else if (ioInfo->buffer[1] == 'G')			//****** ���������û ******
					{//
						cout << "�������� ��û ��Ŷ ���� -> " << sock;
						/*�α׾ƿ� ó���κ� ����� �α׾ƿ��� �������Ḧ ���� ���ָ� ������ �ϴ� ������ ����*/
						send(sock, "@G1", 3, 0);
						cout << "--->(@G1)���� �Ϸ�" << endl;
						CloseClientSock(sock, ioInfo, shareData);
					}
				}//end @
				else if (ioInfo->buffer[0] == '/')
				{	// recv " /[���ȣ]_[Id]_[����] "
					//func(int ���ȣ, char* [ID]_[����]) ->> ���̵�� ������ Ŭ�󿡼� �����ϱ� �׳� ��°�� ������ ��.


					int RoomNum;
					char *tmp;
					tmp = strtok(ioInfo->buffer + 1, "_");
					RoomNum = atoi(tmp);
					cout << "======== WaitingRoom[" << RoomNum << "]Chat Send ========" << endl;
					tmp = strtok(NULL, "");
					SendMsgWaitingRoomFunc(RoomNum, shareData, tmp);
					cout << "======== WaitingRoom[" << RoomNum << "]Chat Send ========" << endl;
					// send " /1_ID_���� " ���⼭ 1�� ���ȣ�� �ƴ϶� send������ �ǹ���
				}
				else if (ioInfo->buffer[0] == '$')
				{
					cout << "���ӽ��� ��Ŷ(" << ioInfo->buffer << ")���� " << endl;
					send(sock, "$1_", 3, 0);
				}
				/*WSARecv*/
				delete ioInfo;
				ioInfo = new OVER_DATA;
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				ioInfo->Mode = READ;

			}

			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;//������ �����͸� ����....?���⼭ wsaBuf�� �ʿ伺������ �˾ƺ����� ����
			WSARecv(sock, &(ioInfo->wsaBuf),
				1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else
		{
			/*gqcs ����
			* - 1. GQCS�� 4��°����(inInfo) �� NULL �� ��� -
			* 2,3�� ���� ���� ���ǵ��� �ʴ´�.
			* �Լ� ��ü ������, Ÿ�Ӿƿ��߻�(WAIT_TIMEOUT) �̳� IOCP�ڵ��� ���� ���(ERROR_ABANDONED_WAIT_0)�� ��� �߻�
			*
			* - 2. GQCS�� 4��° ����(inInfo) �� NULL�� �ƴ� ��� -
			* IOCP�� ����� ��ġ�� ����µ����� ������ �߻��� �����
			* iocpť�κ��� �ش� ����� �Ϸ� ��Ŷ�� dequeue�� ���´�. ���� 2,3�� �Ű����� ��� ������ ������ ä����.
			* ipOverlapped�� Internal �ʵ�� ��ġ�� �����ڵ带 ��� ����.
			* GetLastError�� ȣ���ϸ� �ش� �����ڵ忡 ��ġ�Ǵ� win32 �����ڵ带 ȹ���� �� ����.
			***** NULL�� �ƴ� ��쿡�� �ش� ��ġ �ڵ鿡 ���� ����ó���� ���� �� IOCP�� ����� �ٸ� ��ġ�� ������� �޾Ƶ��̱� ���� ��� ������ ������ �h��. *****
			*/
			DWORD dwErrCode = GetLastError();
			if (ioInfo != NULL)
			{
				switch (dwErrCode)
				{
				case ERROR_NETNAME_DELETED:
					cout << "��Ĺ(" << sock << ')' << " ���� ������: ";
					CloseClientSock(sock, ioInfo, shareData);
					//delete ioInfo;
					break;
				default:
					cout << "[ERROR]GQCS Linked file handle error(" << dwErrCode << ')' << endl;
					break;
				}

				continue;
			}
			else
			{
				if (dwErrCode == WAIT_TIMEOUT)
				{//������ �ð� ���, INFINITE�� �����س��� ������ ���� ������ �ʿ����.
					cout << "GQCS Error: Time out(" << dwErrCode << ')' << endl;
					break;
				}
				else
				{//gqcs ȣ����ü ���� (IOCP�ڵ��� ���� ���)
					cout << "GQCS Error: IOCP handle has been closed" << '(' << dwErrCode << ')' << endl;
					break;
				}
			}
		}
	}//end while
	return 0;
}
const SOCKET ServerClass::GetListenSock(const int Port, const int Backlog = SOMAXCONN)
{
	SOCKET hServSock;
	//SOCKADDR_IN servAdr;//��� ���ε������ ���� �Ⱦ��� ������? �����ϰ� ����������?
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup error(" << WSAGetLastError() << ')' << endl;
		return INVALID_SOCKET;
	}

	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hServSock == INVALID_SOCKET)
	{
		cout << "Socket failed, code(" << WSAGetLastError() << ')' << endl;
		return INVALID_SOCKET;
	}

	//memset(&servAdr, 0, sizeof(servAdr));
	memset(&shareData->servAdr, 0, sizeof(shareData->servAdr));
	shareData->servAdr.sin_family = AF_INET;
	shareData->servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	shareData->servAdr.sin_port = htons(Port);
	LONG lSock;
	lSock = bind(
		hServSock,
		(SOCKADDR*)&shareData->servAdr,
		sizeof(shareData->servAdr)
		);
	if (lSock == SOCKET_ERROR)
	{
		cout << "bind failed, code : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}

	lSock = listen(hServSock, Backlog);

	if (lSock == SOCKET_ERROR)
	{
		cout << "listen failed, code : " << WSAGetLastError() << endl;
		closesocket(hServSock);
		return INVALID_SOCKET;
	}
	return hServSock;
}
void ServerClass::ExitIOCP()
{
	ExitFlag = true;
}
void ServerClass::CloseClientSock(SOCKET sock, LPOVER_DATA ioInfo, LPShared_DATA lpComp)
{

	char CloseName[MAX_NAME_SIZE];
	char tmp[MAX_NAME_SIZE + 128];
	memset(tmp, 0, sizeof(tmp));
	memset(CloseName, 0, sizeof(CloseName));

	list<CLIENT_DATA>::iterator iter;
	iter = lpComp->Clients.begin();
	while (iter != lpComp->Clients.end())
	{
		if (iter->hClntSock == sock)
		{
			EnterCriticalSection(&cs);//cs
			if (iter->MyRoom != 0)
			{
				ExitRoomFunc(lpComp, iter->MyRoom, iter->name);
			}

			strcpy(CloseName, iter->name);
			iter = lpComp->Clients.erase(iter);
			lpComp->Clients_Num--;
			TotalConnectedClientCount--;
			LeaveCriticalSection(&cs);//cs
			break;
		}
		else
		{
			iter++;
		}
	}
	closesocket(sock);

	sprintf(tmp, "[%s] is disconnected...\n", CloseName);
	SendMsgFunc(tmp, lpComp, strlen(tmp));
	cout << tmp;
	//puts("DisConnect Client!");
}
const bool ServerClass::CreateRoomFunc(LPShared_DATA lpComp, SOCKET sock)
{
	ChatRoom room;
	/*
	*RoomName
	*RoomNumber
	*Client_IDs[3]
	*UserCount --> max=3
	*/
	char tmpRoomName[40] = "";
	char CreateRoomSendMsg[100] = "";
	list<CLIENT_DATA>::iterator iter;
	iter = lpComp->Clients.begin();
	while (iter != lpComp->Clients.end())
	{
		if (iter->hClntSock == sock)
		{
			if (iter->MyRoom == 0)
			{

				sprintf(tmpRoomName, "[%s]���� ���Դϴ�.", iter->name);
				strcpy(room.chatRoomName, tmpRoomName);
				room.ChatRoomNum = TotalCreateRoomCount + 1;
				strcpy(room.ClientsID[0], iter->name);
				memset(room.ClientsID[1], 0, sizeof(room.ClientsID[1]));
				memset(room.ClientsID[2], 0, sizeof(room.ClientsID[2]));
				room.hClntSock[0] = sock;//����¾ֵ� ��Ĺ �־������!!
				room.UserCount = 1;

				iter->MyRoom = room.ChatRoomNum;

				EnterCriticalSection(&cs);//cs
				lpComp->ChatRoomList.push_back(room);
				TotalCreateRoomCount++;
				LeaveCriticalSection(&cs);//cs
				//�� ��������׸� ������ ����

				sprintf(CreateRoomSendMsg, "@R1_%d_%s", room.ChatRoomNum, room.chatRoomName);
				send(sock, CreateRoomSendMsg, strlen(CreateRoomSendMsg), 0);
				cout << "�� �����Ϸ�>>";
				cout << CreateRoomSendMsg << "���� ����" << endl;
				//============================��������Ʈ ���ۺκ�============================
				/*�̺κ��� Ŭ�󿡼� ó�� �� ���鶧 ID�� �ڱ� ���� �濡 �����ϱ� ������ ���� �Ӻ����� �ʿ���µ� ���߿� Ȯ���غ��� ������*/
				memset(CreateRoomSendMsg, 0, sizeof(CreateRoomSendMsg));
				cout << "================��������Ʈ ���ۺ�================" << endl;
				cout << CreateRoomSendMsg << endl;
				sprintf(CreateRoomSendMsg, "@U1_%s-10_", iter->name);
				cout << CreateRoomSendMsg << " >> ����" << endl;
				send(sock, CreateRoomSendMsg, strlen(CreateRoomSendMsg), 0);
				//============================��������Ʈ ���ۺκ�============================
				return true;
			}
			else
			{
				return false;//�̹� �濡 ������
			}
		}
		else
		{
			iter++;
		}
	}
	return false;//ID�� ã���� ����.


}
const bool ServerClass::ExitRoomFunc(LPShared_DATA lpComp, int RoomNum, char *id)
{//�� �Լ��� �׻� cs�ȿ��־����
	list<ChatRoom>::iterator iter_room;
	list<CLIENT_DATA>::iterator iter_user;
	iter_user = lpComp->Clients.begin();
	while (iter_user != lpComp->Clients.end())
	{
		if (strcmp(iter_user->name, id) == 0)
		{
			cout << id << " :�˻� �Ϸ�: " << iter_user->name << endl;
			break;
		}
		else iter_user++;
	}
	iter_room = lpComp->ChatRoomList.begin();
	while (iter_room != lpComp->ChatRoomList.end())
	{
		if (iter_room->ChatRoomNum == RoomNum)
		{
			if (iter_room->UserCount <= 1)
			{//�濡 �����̾����� �׳� �� ����
				iter_room = lpComp->ChatRoomList.erase(iter_room);
				//���� ���������� ���ο� �� �������� ��� Ŭ�� send
				iter_user->MyRoom = 0;
				SendWaitingRoomList(lpComp);
				return true;
			}
			else
			{//�濡 ������ �ٸ������ ������....�׳� ���� ����
				for (int i = 0; i < 3; i++)
				{//���� ID����Ʈ���� �ش�ID ����� ����
					if (strcmp(iter_room->ClientsID[i], id) == 0)
					{
						memset(iter_room->ClientsID[i], 0, sizeof(iter_room->ClientsID[i]));
						for (int j = i; j < 3; j++)
						{//����� ��Ĺ�� �߰�����
							if (j == 3 - 1)
							{
								memset(iter_room->ClientsID[j], 0, sizeof(iter_room->ClientsID[j]));
								iter_room->hClntSock[j] = NULL;
							}
							else
							{
								strcpy(iter_room->ClientsID[j], iter_room->ClientsID[j + 1]);
								iter_room->hClntSock[j] = iter_room->hClntSock[j + 1];
							}
						}
					}
				}
				iter_room->UserCount--;
				iter_user->MyRoom = 0;
				//============================��������Ʈ ���ۺκ�============================
				char ExitRoomSendMsg[100] = "";
				int MsgSz;
				memset(ExitRoomSendMsg, 0, sizeof(ExitRoomSendMsg));
				for (int i = 0; i < iter_room->UserCount; i++)
				{//@U1_alalssk-5_test-10_
					if (i == 0)strcpy(ExitRoomSendMsg, "@U1_");
					else strcat(ExitRoomSendMsg, "_");
					strcat(ExitRoomSendMsg, iter_room->ClientsID[i]);
					strcat(ExitRoomSendMsg, "-");
					strcat(ExitRoomSendMsg, "10");//�ӽ�winCount
				}
				strcat(ExitRoomSendMsg, "_");

				MsgSz = strlen(ExitRoomSendMsg);
				for (int i = 0; i < iter_room->UserCount; i++)
				{
					cout << '[' << iter_room->hClntSock[i] << "] �泪���� ��������Ʈ ���� >> " << ExitRoomSendMsg << endl;
					//����ī��Ʈ�� ���̳ʽ��ǰ� ������ �迭�� �ȹٲ��
					send(iter_room->hClntSock[i], ExitRoomSendMsg, MsgSz, 0);
				}
				//============================��������Ʈ ���ۺκ�============================
				return true;
			}

		}
		else iter_room++;
	}
	return false;
}
const bool ServerClass::JoinRoomFunc(LPShared_DATA lpComp, SOCKET sock, int RoomNum)
{
	char JoinRoomSendMsg[100] = "";
	int MsgSz;
	list<ChatRoom>::iterator iter_room;
	list<CLIENT_DATA>::iterator iter_user;
	iter_room = lpComp->ChatRoomList.begin();
	while (iter_room != lpComp->ChatRoomList.end())
	{
		if (iter_room->ChatRoomNum == RoomNum)
		{
			break;
		}
		else iter_room++;
	}
	if (iter_room == lpComp->ChatRoomList.end())
	{//���� ��ã������
		cout << "���� ã�� ����" << endl;
		return false;
	}
	else if (iter_room->UserCount >= 3)
	{//���� ã������ �������
		cout << "���� ����" << endl;
		return false;
	}
	else
	{
		iter_user = lpComp->Clients.begin();
		while (iter_user != lpComp->Clients.end())
		{
			if (iter_user->hClntSock == sock)
			{
				EnterCriticalSection(&cs);//cs
				iter_user->MyRoom = RoomNum;
				strcpy(iter_room->ClientsID[iter_room->UserCount], iter_user->name);
				iter_room->hClntSock[iter_room->UserCount++] = iter_user->hClntSock;
				LeaveCriticalSection(&cs);//cs
				sprintf(JoinRoomSendMsg, "@J1_%d_%s", iter_room->ChatRoomNum, iter_room->chatRoomName);
				send(sock, JoinRoomSendMsg, strlen(JoinRoomSendMsg), 0);
				cout << "�� ����Ϸ� >> ";
				cout << JoinRoomSendMsg << "���� ����" << endl;

				//====================��������Ʈ ���ۺκ�============================
				memset(JoinRoomSendMsg, 0, sizeof(JoinRoomSendMsg));
				for (int i = 0; i < iter_room->UserCount; i++)
				{//@U1_alalssk-5_test-10_
					if (i == 0)strcpy(JoinRoomSendMsg, "@U1_");
					else strcat(JoinRoomSendMsg, "_");
					strcat(JoinRoomSendMsg, iter_room->ClientsID[i]);
					strcat(JoinRoomSendMsg, "-");
					strcat(JoinRoomSendMsg, "10");//�ӽ�winCount
				}
				strcat(JoinRoomSendMsg, "_");

				MsgSz = strlen(JoinRoomSendMsg);
				for (int i = 0; i < iter_room->UserCount; i++)
				{
					cout << '[' << iter_room->hClntSock[i] << "]���� ��������Ʈ ���� >> " << JoinRoomSendMsg << endl;
					send(iter_room->hClntSock[i], JoinRoomSendMsg, MsgSz, 0);
				}
				//====================��������Ʈ ���ۺκ�============================
				return true;
			}
			else iter_user++;
		}
		if (iter_user == lpComp->Clients.end())
		{
			cout << "�������� ������ �ƴմϴ�" << endl;
			return false;
		}
	}
	return false;
}
void ServerClass::Print_UserList()
{
	list<CLIENT_DATA>::iterator iter;
	iter = shareData->Clients.begin();
	while (iter != shareData->Clients.end())
	{
		cout << '[' << iter->name << ']';
		iter++;
	}
	cout << endl;

}
void ServerClass::Print_RoomList()
{
	list<ChatRoom>::iterator iter;
	iter = shareData->ChatRoomList.begin();
	cout << shareData->ChatRoomList.size() << "���� ����" << endl;
	while (iter != shareData->ChatRoomList.end())
	{
		cout << '[' << iter->chatRoomName << ']' << endl;
		iter++;
	}
}
bool ServerClass::SendWaitingRoomList(LPShared_DATA lpComp)
{
	char SendWaitingRoomListBuf[40 * 9999] = "";
	char cRoomNum[5] = "";

	list<ChatRoom>::iterator iter;
	iter = lpComp->ChatRoomList.begin();
	sprintf(SendWaitingRoomListBuf, "!%d", lpComp->ChatRoomList.size());
	while (iter != lpComp->ChatRoomList.end())
	{
		strcat(SendWaitingRoomListBuf, "_No.");
		_itoa(iter->ChatRoomNum, cRoomNum, 10);//itoa( ���ڰ� , ���� �� string �迭 , ���ڰ��� ��ȯ�� ���� ) 
		strcat(SendWaitingRoomListBuf, cRoomNum);
		strcat(SendWaitingRoomListBuf, ">>");
		strcat(SendWaitingRoomListBuf, iter->chatRoomName);
		iter++;
	}
	cout << SendWaitingRoomListBuf;
	SendMsgFunc(SendWaitingRoomListBuf, lpComp, strlen(SendWaitingRoomListBuf));
	cout << "::�� ����Ʈ ������ �Ϸ�" << endl;
	return true;
}
void ServerClass::SendMsgFunc(char* buf, LPShared_DATA lpComp, DWORD SnedSz)
{
	list<CLIENT_DATA>::iterator iter;
	iter = lpComp->Clients.begin();
	while (iter != lpComp->Clients.end())
	{
		send(iter->hClntSock, buf, SnedSz, 0);
		iter++;
	}
}
void ServerClass::SendMsgWaitingRoomFunc(int RoomNum, LPShared_DATA lpComp, char* msg)
{
	list<ChatRoom>::iterator iter_room;
	char SendMsg[1024] = "";
	iter_room = lpComp->ChatRoomList.begin();
	while (iter_room != lpComp->ChatRoomList.end())
	{
		if (iter_room->ChatRoomNum == RoomNum)
		{
			break;
		}
		else iter_room++;
	}
	sprintf(SendMsg, "/1_%s", msg);
	for (int i = 0; i < iter_room->UserCount; i++)
	{
		send(iter_room->hClntSock[i], SendMsg, strlen(SendMsg), 0);
		cout << '[' << iter_room->hClntSock[i] << "] >> " << SendMsg << endl;
	}
}
void ServerClass::PrintRoomInfo()
{
	char cRoomName[50] = "";
	int iRoomNameSz;
	list<ChatRoom>::iterator iter_room;
	iter_room = shareData->ChatRoomList.begin();
	while (iter_room != shareData->ChatRoomList.end())
	{
		strcpy(cRoomName, iter_room->chatRoomName);
		iRoomNameSz = strlen(cRoomName);
		cout << '+';
		for (int i = 0; i < iRoomNameSz + 6; i++)
		{
			cout << '-';
		}
		cout << '+' << endl;
		cout << "No." << iter_room->ChatRoomNum << " " << iter_room->chatRoomName << endl;
		cout << iter_room->UserCount << " �� ������" << endl;
		for (int i = 0; i < iter_room->UserCount; i++)
		{
			cout << i + 1 << ". " << iter_room->ClientsID[i] << endl;
		}
		cout << '+';
		for (int i = 0; i < iRoomNameSz + 6; i++)
		{
			cout << '-';
		}
		cout << '+' << endl;
		iRoomNameSz = 0;
		memset(cRoomName, 0, sizeof(cRoomName));
		iter_room++;
	}
}