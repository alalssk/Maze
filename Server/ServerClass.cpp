#include "ServerClass.h"
int ServerClass::TotalConnectedClientCount = 0;
int ServerClass::TotalCreateRoomCount = 0;
bool ServerClass::ExitFlag = false;
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
	//TotalCreateRoomCount �޾ƿ�
	TotalCreateRoomCount = sDB.GetTotalCreateRoomCount();
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

	while (1)
	{
		BOOL bGQCS = GetQueuedCompletionStatus(
			shareData->hComPort,
			&bytesTrans,
			&sock,//���ø���Ű
			(LPOVERLAPPED*)&ioInfo,
			INFINITE
			);//error���� �� ������� 
		if (bGQCS && ioInfo)//gqcs ����
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
					sDB.GetUserWinCount("alalssk", client_data.win_count, client_data.play_count);
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


				PacketType type = GetPacketTypeFromClient(ioInfo->buffer);

				if (PacketType::REQUEST_CREATE_ROOM == type)
				{

					if (CreateRoomFunc(shareData, sock))
					{
						SendWaitingRoomList(shareData);
					}
					else
					{
						cout << "�� ���� ���� -> " << sock << endl;
						send(sock, "@R0", 3, 0);
					}
				}
				else if (PacketType::REQUEST_ROOM_LIST == type)
				{
					send(sock, "@r1", 3, 0);
					SendWaitingRoomList(shareData);
				}
				else if (PacketType::REQUEST_JOIN_ROOM == type)
				{
					char *cRoomNum;
					int iRoomNum;
					strtok(ioInfo->buffer, "_");
					cRoomNum = strtok(NULL, "_");
					iRoomNum = atoi(cRoomNum);
					cout << "�� ���� ��Ŷ ����" << iRoomNum << endl;
					if (JoinRoomFunc(shareData, sock, iRoomNum))
					{
						cout << "�����强��(@J1) ����->" << sock << endl;
					}
					else
					{
						send(sock, "@J0", 3, 0);
						cout << "���������(@J0) ����->" << sock << endl;
					}
				}
				else if (PacketType::REQUEST_EXIT_ROOM == type)
				{
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
				else if (PacketType::REQUEST_LOGOUT == type)
				{
					cout << "�α׾ƿ� ��û ��Ŷ ���� -> " << sock;
					send(sock, "@L1", 3, 0);
					cout << "--->(@L1)���� �Ϸ�" << endl;
					CloseClientSock(sock, ioInfo, shareData);
				}
				else if (PacketType::REQUEST_GAME_EXIT == type)
				{
					cout << "�������� ��û ��Ŷ ���� -> " << sock;
					send(sock, "@G1", 3, 0);
					cout << "--->(@G1)���� �Ϸ�" << endl;
					CloseClientSock(sock, ioInfo, shareData);
				}
				else if (PacketType::ROOM_CHAT == type)
				{
					int RoomNum;
					char *tmp;
					tmp = strtok(ioInfo->buffer + 1, "_");
					RoomNum = atoi(tmp);
					cout << "======== WaitingRoom[" << RoomNum << "]Chat Send ========" << endl;
					tmp = strtok(NULL, "");
					SendMsgWaitingRoomFunc(RoomNum, shareData, tmp);
				}
				else if (PacketType::REQUEST_START_GAME == type)
				{
					SetStartRoom(shareData, atoi(ioInfo->buffer + 3));
					//Ŭ��κ��� ���� ���ӽ��ۿ�û $R_���ȣ �� ���� �������� ��� Ŭ��� ����
					cout << '[' << sock << "]���ӽ��� ��Ŷ ���� >> " << ioInfo->buffer << endl;
					SendMsgFunc(ioInfo->buffer, shareData, strlen(ioInfo->buffer) + 1);
				}
				else if (PacketType::REQUEST_READY_GAME == type)
				{
					//$S[���ȣ]_[ID] �� ������
					//$S1_[ID]�� ��������
					cout << '[' << sock << "]���� �غ� �Ϸ� ��Ŷ ���� >> " << ioInfo->buffer << endl;
					SendUserState(shareData, ioInfo->buffer + 2);
				}
				else if (PacketType::MOVE == type)
				{
					cout << "GamePlay ��Ŷ ���� >> " << ioInfo->buffer;
					char *tmp;
					int iRoomNum, UserKey, InputKey;
					tmp = strtok(ioInfo->buffer + 1, "_");
					iRoomNum = atoi(tmp);
					tmp = strtok(NULL, "_");
					UserKey = atoi(tmp);
					tmp = strtok(NULL, "");
					InputKey = atoi(tmp);
					cout << " >> [" << iRoomNum << "][" << UserKey << "][" << InputKey << ']' << endl;
					SendUserInputKey_GamePlay(shareData, iRoomNum, UserKey, InputKey);
				}
				else if (PacketType::USER_GAME_FINISH == type)
				{
					list<ChatRoom>::iterator iter_game;
					int iRoomNum, Mykey;
					char *tmp;
					char SendMsg[20] = "";
					int SendMsgSz = 0;
					tmp = strtok(ioInfo->buffer + 1, "_");
					iRoomNum = atoi(tmp);
					tmp = strtok(NULL, "");
					Mykey = atoi(tmp);
					iter_game = shareData->GameRoomList.begin();
					sprintf(SendMsg, "Q%d", Mykey);
					SendMsgSz = strlen(SendMsg);
					while (iter_game != shareData->GameRoomList.end())
					{
						if (iter_game->ChatRoomNum == iRoomNum)
						{
							for (int i = 0; i < iter_game->UserCount; i++)
							{
								send(iter_game->hClntSock[i], SendMsg, SendMsgSz + 1, 0);
							}
							cout << SendMsg << "�� ��⸦ ���½��ϴ�" << endl;
							break;
						}
						else iter_game++;
					}
				}
				else if (PacketType::GAME_OVER == type)
				{
					cout << "[������ �����ٴ� ��ȣ] >>>" << ioInfo->buffer << endl;
					DeleteStartRoom(shareData, atoi(ioInfo->buffer + 1));
					PlusWinCount(shareData, sock);
				}
				/*����κ�*/
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
bool ServerClass::CloseClientSock(SOCKET sock, LPOVER_DATA ioInfo, LPShared_DATA lpComp)
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

			closesocket(sock);
			sDB.UserConnection(CloseName, "LOGOUT");
			sprintf(tmp, "[%s] is disconnected...\n", CloseName);
			cout << tmp;
			//SendMsgFunc(tmp, lpComp, strlen(tmp)); �������� �޽����� Ŭ�� ������ִ°� �����ϱ�����.
			//puts("DisConnect Client!");

			return true;
		}
		else
		{
			iter++;
		}
	}
	return false;
}
const bool ServerClass::CreateRoomFunc(LPShared_DATA lpComp, SOCKET sock)
{
	ChatRoom room;
	char tmpRoomName[255] = "";
	char CreateRoomSendMsg[100] = "";
	list<CLIENT_DATA>::iterator iter_user;
	iter_user = lpComp->Clients.begin();

	while (iter_user != lpComp->Clients.end())
	{
		if (iter_user->hClntSock == sock)
		{
			if (iter_user->MyRoom == 0)
			{

				sprintf(tmpRoomName, "This room was made by %s.", iter_user->name);
				strcpy(room.chatRoomName, tmpRoomName);
				room.ChatRoomNum = TotalCreateRoomCount + 1;
				strcpy(room.ClientsID[0], iter_user->name);
				memset(room.ClientsID[1], 0, sizeof(room.ClientsID[1]));
				memset(room.ClientsID[2], 0, sizeof(room.ClientsID[2]));
				room.hClntSock[0] = sock;//����¾ֵ� ��Ĺ �־������!!
				room.UserCount = 1;
				room.UserState[0] = false;
				room.UserState[1] = false;
				room.UserState[2] = false;
				iter_user->MyRoom = room.ChatRoomNum;

				EnterCriticalSection(&cs);//cs
				lpComp->ChatRoomList.push_back(room);
				TotalCreateRoomCount++;
				sDB.CreateWaitingRoom(tmpRoomName, room.ChatRoomNum);//DB >> room_tbl
				sDB.JoinWaitingRoom(room.ChatRoomNum, iter_user->name);//DB >> visit_room_tbl
				LeaveCriticalSection(&cs);//cs
				//�� ��������׸� ������ ����

				sprintf(CreateRoomSendMsg, "@R1_%d_%s", room.ChatRoomNum, room.chatRoomName);
				send(sock, CreateRoomSendMsg, strlen(CreateRoomSendMsg), 0);
				cout << "�� �����Ϸ�>>";
				cout << CreateRoomSendMsg << "���� ����" << endl;
				//============================��������Ʈ ���ۺκ�============================
				/*�̺κ��� Ŭ�󿡼� ó�� �� ���鶧 ID�� �ڱ� ���� �濡 �����ϱ� ������ ���� �Ӻ����� �ʿ���µ� ���߿� Ȯ���غ��� ������*/
				memset(CreateRoomSendMsg, 0, sizeof(CreateRoomSendMsg));
				cout << CreateRoomSendMsg << endl;
				sprintf(CreateRoomSendMsg, "@U1_%s-%d", iter_user->name, iter_user->win_count);
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
			iter_user++;
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
				sDB.DeleteWaitingRoom(RoomNum);//db >> room_tbl
				sDB.ExitWaitingRoom(RoomNum, iter_user->name);//DB >> visit_room_tbl
				return true;
			}
			else
			{//�濡 ������ �ٸ������ ������....�׳� ���� ����
				for (int i = 0; i < MAX_USERNUM; i++)
				{//���� ID����Ʈ���� �ش�ID ����� ����
					if (strcmp(iter_room->ClientsID[i], id) == 0)
					{
						memset(iter_room->ClientsID[i], 0, sizeof(iter_room->ClientsID[i]));
						for (int j = i; j < MAX_USERNUM; j++)
						{//����� ��Ĺ�� �߰�����
							if (j == MAX_USERNUM - 1)
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
				sDB.ExitWaitingRoom(RoomNum, iter_user->name);//DB >> visit_room_tbl

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
				//		strcat(ExitRoomSendMsg, "_");

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
	char cWinCount[5] = "";
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
	else if (iter_room->UserCount >= MAX_USERNUM)
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
				sDB.JoinWaitingRoom(RoomNum, iter_user->name);// DB >> visit_room_tbl
				LeaveCriticalSection(&cs);//cs
				sprintf(JoinRoomSendMsg, "@J1_%d_%s", iter_room->ChatRoomNum, iter_room->chatRoomName);
				send(sock, JoinRoomSendMsg, strlen(JoinRoomSendMsg), 0);
				cout << "�� ����Ϸ� >> ";
				cout << JoinRoomSendMsg << "���� ����" << endl;

				//====================��������Ʈ ���ۺκ�============================
				memset(JoinRoomSendMsg, 0, sizeof(JoinRoomSendMsg));
				for (int i = 0; i < iter_room->UserCount; i++)
				{//@U1_alalssk-5_test-10
					if (i == 0)strcpy(JoinRoomSendMsg, "@U1_");
					else strcat(JoinRoomSendMsg, "_");
					strcat(JoinRoomSendMsg, iter_room->ClientsID[i]);
					strcat(JoinRoomSendMsg, "-");
					_itoa(iter_user->win_count, cWinCount, 10);
					strcat(JoinRoomSendMsg, cWinCount);
					memset(cWinCount, 0, sizeof(cWinCount));
				}


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
bool ServerClass::SendUserState(LPShared_DATA lpComp, char *input)
{//���ȣ_���̵�
	char *tmp;
	int InputRoomNum = 0;
	char InputUserID[13] = "";
	char SendMsg[100] = "";
	tmp = strtok(input, "_");
	InputRoomNum = atoi(tmp);
	tmp = strtok(NULL, "_");
	strcpy(InputUserID, tmp);

	list<ChatRoom>::iterator iter_room;
	iter_room = lpComp->ChatRoomList.begin();
	while (iter_room != lpComp->ChatRoomList.end())
	{
		if (iter_room->ChatRoomNum == InputRoomNum)
		{
			for (int i = 0; i < iter_room->UserCount; i++)
			{
				if (strcmp(iter_room->ClientsID[i], InputUserID) == 0)
				{
					iter_room->UserState[i] = true;
					//$S1_[ID]�� ��������
					sprintf(SendMsg, "$S1_%s", InputUserID);
					for (int j = 0; j < iter_room->UserCount; j++)
					{
						send(iter_room->hClntSock[j], SendMsg, strlen(SendMsg) + 1, 0);
						cout << '[' << iter_room->hClntSock[j] << "] ���º��� �޽��� ���� >> " << InputUserID << "�� ���¸� �����϶� Ŭ����̿�" << endl;
					}

					return true;
				}
			}
		}
		else iter_room++;
	}

	return false;
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

const bool ServerClass::SetStartRoom(LPShared_DATA lpComp, int RoomNum) //������ ���۵� �� ����Ʈ �߰�
{
	list<ChatRoom>::iterator iter_room;
	iter_room = lpComp->ChatRoomList.begin();
	ChatRoom room_game;
	while (iter_room != lpComp->ChatRoomList.end())
	{
		if (iter_room->ChatRoomNum == RoomNum)
		{
			strcpy(room_game.chatRoomName, iter_room->chatRoomName);
			room_game.ChatRoomNum = iter_room->ChatRoomNum;
			room_game.UserCount = iter_room->UserCount;
			for (int i = 0; i < room_game.UserCount; i++)
			{
				strcpy(room_game.ClientsID[i], iter_room->ClientsID[i]);
				room_game.hClntSock[i] = iter_room->hClntSock[i];
				room_game.UserState[i] = iter_room->UserState[i];
			}

			sDB.StartPlayGame(RoomNum);							//DB >> game_tbl(insert)
			room_game.gameID = sDB.GetPlayingGameID(RoomNum);	//DB >> game_tbl(select)
			for (int i = 0; i < room_game.UserCount; i++)		//DB >> play_game_tbl(insert)
			{
				sDB.GamePlayUserLog(room_game.gameID, iter_room->ClientsID[i]);
			}

			lpComp->GameRoomList.push_back(room_game);

			return true;
		}
		else iter_room++;
	}
	return false;
}
//const bool ServerClass::SetStartRoom(LPShared_DATA lpComp, int RoomNum) //splice �� ���� Ȯ���ϰ� �˰� ����
//{
//	list<ChatRoom>::iterator iter_room;
//	iter_room = lpComp->ChatRoomList.begin();
//	list<ChatRoom>::iterator iter_game;
//	iter_game = lpComp->GameRoomList.begin();
//	while (iter_room != lpComp->ChatRoomList.end())
//	{
//		if (iter_room->ChatRoomNum == RoomNum)
//		{
//			lpComp->GameRoomList.splice(lpComp->GameRoomList.begin(), lpComp->ChatRoomList, iter_room);
//			return true;
//		}
//		else iter_room++;
//	}
//	return false;
//}

//�������� �� �˻� (Ŭ�� send���ִ� �Լ�) �����

void ServerClass::Print_GameRoomList()//�������� �� ��� �Լ��� ����� main��
{
	list<ChatRoom>::iterator iter_game;
	iter_game = shareData->GameRoomList.begin();
	while (iter_game != shareData->GameRoomList.end())
	{
		cout << '[' << iter_game->chatRoomName << "]";

		if (++iter_game == shareData->GameRoomList.end()) cout << endl;
		else cout << '-';
	}
}
const bool ServerClass::DeleteStartRoom(LPShared_DATA lpComp, int RoomNum)
{
	list<ChatRoom>::iterator iter_game;
	iter_game = lpComp->GameRoomList.begin();
	while (iter_game != lpComp->GameRoomList.end())
	{
		if (iter_game->ChatRoomNum == RoomNum)
		{
			sDB.EndPlayGame(iter_game->gameID);				//DB >> game_tbl(update)
			for (int i = 0; i < iter_game->UserCount; i++)	//DB >> user_tbl(update)
			{
				sDB.PlusPlayCount(iter_game->ClientsID[i]);
			}
			iter_game = lpComp->GameRoomList.erase(iter_game);

			return true;
		}
		else iter_game++;
	}
}

void ServerClass::SendUserInputKey_GamePlay(LPShared_DATA lpComp, int room, int Userkey, int InputKey)
{
	list<ChatRoom>::iterator iter_game;
	char SendMsg[20] = "";
	int SendMsgSz = 0;
	iter_game = lpComp->GameRoomList.begin();
	while (iter_game != lpComp->GameRoomList.end())
	{
		if (iter_game->ChatRoomNum == room)
		{
			sprintf(SendMsg, "P%d_%d", Userkey, InputKey);
			SendMsgSz = strlen(SendMsg);
			for (int i = 0; i < iter_game->UserCount; i++)
			{
				send(iter_game->hClntSock[i], SendMsg, SendMsgSz + 1, 0);
				cout << '[' << iter_game->hClntSock[i] << "] GamePlay ��Ŷ ���� >> " << SendMsg << endl;
			}
			break;
		}
		else iter_game++;
	}
}
const bool ServerClass::PlusWinCount(LPShared_DATA lpComp, SOCKET sock)
{
	list<CLIENT_DATA>::iterator iter_user;
	iter_user = lpComp->Clients.begin();
	while (iter_user != lpComp->Clients.end())
	{
		if (iter_user->hClntSock == sock)
		{
			iter_user->win_count++;
			sDB.PlusWinCount(iter_user->name);	//DB user_tbl >> update
			return true;
		}
		else iter_user++;
	}
	return false;
}
PacketType ServerClass::GetPacketTypeFromClient(char * buffer)
{
	if (buffer[0] == '@')
	{
		switch (buffer[1])
		{
		case 'R':
			return PacketType::REQUEST_CREATE_ROOM;
			break;
		case 'r':
			return PacketType::REQUEST_ROOM_LIST;
			break;
		case 'J':
			return PacketType::REQUEST_JOIN_ROOM;
			break;
		case 'E':
			return PacketType::REQUEST_EXIT_ROOM;
			break;
		case 'L':
			return PacketType::REQUEST_LOGOUT;
			break;
		case 'G':
			return PacketType::REQUEST_GAME_EXIT;
			break;
		}
	}
	else if (buffer[0] == '/')
	{
		return PacketType::ROOM_CHAT;
	}
	else if (buffer[0] == '$')
	{
		switch (buffer[1])
		{
		case 'R':
			return PacketType::REQUEST_START_GAME;
			break;
		case 'S':
			return PacketType::REQUEST_READY_GAME;
			break;
		}
	}
	else
	{
		switch (buffer[0])
		{
		case 'P':
			return PacketType::MOVE;
			break;
		case 'Q':
			return PacketType::USER_GAME_FINISH;
			break;
		case 'q':
			return PacketType::GAME_OVER;
			break;
		}
	}

	return PacketType::NONE;
}
