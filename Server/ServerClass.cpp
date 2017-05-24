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

	list<CLIENT_DATA>::iterator iter;
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
					if (ioInfo->buffer[1] == 'R')	//�� ���� ��û�� ����
					{								//�� ���� �Ϸ� �� ��� Ŭ���̾�Ʈ�� ���ο� ��鿡 ���� ������ send��
						if (CreateRoomFunc(shareData, sock))//�� ����
						{//�� �����Ҷ��� ���������
							//�׸��� ���Ŭ�� ���ο� ������(!"�氳��"_"No.[���ȣ]>> [���̸�]"_... send

							cout << "�� �����Ϸ�" << endl;
							send(sock, "��", 3, 0);//��������Ŷ
							send(sock, "@R1", 3, 0);
							SendWaitingRoomList(shareData);
						}
						else 
						{
							cout << "�� ���� ����" << endl;
							send(sock, "��", 3, 0);//��������Ŷ
							send(sock, "@R0", 3, 0);
						}

						
					}
					else if (ioInfo->buffer[1] == 'J')	//�� ���� ��û Join
					{//@J_[���ȣ]
						char *cRoomNum;
						int iRoomNum;
						strtok(ioInfo->buffer, "_");
						cRoomNum = strtok(NULL, "_");
						iRoomNum = atoi(cRoomNum);
						cout << "�� ���� ��Ŷ ����" << iRoomNum << endl;
						JoinRoomFunc(shareData, sock, iRoomNum);
						//func(shareData, sock, roomNum)


					}
					else if (ioInfo->buffer[1] == 'E')//�� ������ ��û Exit
					{//@E_[���ȣ]_[ID]
						cout << "�� ������ ��û ��Ŷ ����" << endl;
					}
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
					cout << "��Ĺ ���� ������: ";
					CloseClientSock(sock, ioInfo, shareData);
					break;
				default:
					cout << "GQCS Linked file handle error(" << dwErrCode << ')' << endl; break;
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

	sprintf(tmp, "/[%s] is disconnected...\n", CloseName);
	SendMsgFunc(tmp, lpComp, strlen(tmp));
	delete ioInfo;
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
				room.UserCount = 1;

				iter->MyRoom = room.ChatRoomNum;

				EnterCriticalSection(&cs);//cs
				lpComp->ChatRoomList.push_back(room);
				TotalCreateRoomCount++;
				LeaveCriticalSection(&cs);//cs
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
	list<ChatRoom>::iterator iter;
	iter = lpComp->ChatRoomList.begin();
	while (iter != lpComp->ChatRoomList.end())
	{
		if (iter->ChatRoomNum == RoomNum)
		{
			if (iter->UserCount <= 1)
			{//�濡 �����̾����� �׳� �� ����
				iter = lpComp->ChatRoomList.erase(iter);
				//���� ���������� ���ο� �� �������� ��� Ŭ�� send
				return true;
			}
			else
			{//�濡 ������ �ٸ������ ������....�׳� ���� ����
				for (int i = 0; i < 3; i++)
				{//���� ID����Ʈ���� �ش�ID ����� ����
					if (strcmp(iter->ClientsID[i], id) == 0)
					{
						memset(iter->ClientsID[i], 0, sizeof(iter->ClientsID[i]));
						for (int j = i; j < 3; j++)
						{
							if (j == 3 - 1)memset(iter->ClientsID[j], 0, sizeof(iter->ClientsID[j]));
							else strcpy(iter->ClientsID[j], iter->ClientsID[j + 1]);
						}
					}
				}
				iter->UserCount--;
			}
		
		}
		else iter++;
	}
	return false;
}
const bool ServerClass::JoinRoomFunc(LPShared_DATA lpComp, SOCKET sock, int RoomNum)
{
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
				strcpy(iter_room->ClientsID[iter_room->UserCount++], iter_user->name);
				LeaveCriticalSection(&cs);//cs
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
		cout << '[' << iter->chatRoomName << ']'<<endl;
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
	cout << SendWaitingRoomListBuf << endl;
	SendMsgFunc(SendWaitingRoomListBuf, lpComp, strlen(SendWaitingRoomListBuf));
	cout << "�� ����Ʈ ������ �Ϸ�" << endl;
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