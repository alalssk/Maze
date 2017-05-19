#include "stdafx.h"
#include "ServerClass.h"
int ServerClass::TotalConnectedClientCount = 0;
bool ServerClass::ExitFlag = false;
int ServerClass::ChatRoomCount = 0;
LogClass ServerClass::Chatlog;
LogClass ServerClass::DBLog;
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
bool ServerClass::ServerClassMain()
{
	Chatlog.OpenFile("ChatLog.txt");
	DBLog.OpenFile("DBLog.txt");
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
	hTheards[0] = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, (void*)shareData, 0, NULL);//Begin Accept Thread
	cout << "servermain ok" << endl;

	return true;
}
unsigned ServerClass::AcceptThread(PVOID pComPort)
{
	//SOCKET ServSock = (SOCKET)pServSock;
	LPShared_DATA lpComPort = (LPShared_DATA)pComPort;
	CLIENT_DATA client_data;
	//LPCLIENT_DATA lpclient_data;
	int addrLen = sizeof(client_data.clntAdr);
	CRITICAL_SECTION cs;
	InitializeCriticalSection(&cs);
	/*ũ��Ƽ�û��� ��ü�� ��ƾ���� �����ؼ� �ؾ��ϳ�...?
	�Ѱ��� �����ؼ� �ؾ��ϳ�*/
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
		/*����� ����ó���� ���ؼ��� �񵿱� accept�� �ؾ��Ѵ� ���?*/
		/*���� �񵿱� ó���� �� �ʿ䰡�ֳ�...? ���� �����Ҷ� accept�����带 ����Ű�ø� �����ʳ�..?*/

		EnterCriticalSection(&cs);
		cout << "Accept ó����..." << endl;

		lpComPort->Clients.push_back(client_data);//list
		lpComPort->Clients_Num++;
		TotalConnectedClientCount++;
		LeaveCriticalSection(&cs);
		if (CreateIoCompletionPort(
			(HANDLE)client_data.hClntSock,
			lpComPort->hComPort,
			(DWORD)client_data.hClntSock,
			0
			) == NULL)//���ø���Ű�� Ŭ���Ĺ�� �־���
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
		cout << "client sock: " << client_data.hClntSock << endl;
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
void ServerClass::SendMsgFunc(char* buf, LPShared_DATA lpComPort, DWORD RecvSz)
{
	list<CLIENT_DATA>::iterator iter;
	iter = lpComPort->Clients.begin();
	while (iter != lpComPort->Clients.end())
	{
		send(iter->hClntSock, buf, RecvSz, 0);
		iter++;//���������� ����
	}
}
unsigned  __stdcall ServerClass::IOCPWorkerThread(LPVOID CompletionPortIO)
{
	//HANDLE hComPort = (HANDLE)pComPort;
	LPShared_DATA shareData = (LPShared_DATA)CompletionPortIO;
	SOCKET sock;
	DWORD bytesTrans;
	LPOVER_DATA ioInfo;
	DWORD flags = 0;
	CRITICAL_SECTION cs;
	char SendMsg[1024];
	char clntName[MAX_NAME_SIZE];
	memset(SendMsg, 0, (sizeof(SendMsg)));
	memset(clntName, 0, (sizeof(clntName)));
	InitializeCriticalSection(&cs);

	list<CLIENT_DATA>::iterator iter;
	list<ChatRoom>::iterator iter_ChatRoom;
	while (1)
	{
		BOOL bGQCS = GetQueuedCompletionStatus(
			shareData->hComPort,
			&bytesTrans,
			&sock,
			(LPOVERLAPPED*)&ioInfo,
			INFINITE
			);//error���� �� ������� 
		if (bGQCS)//gqcs ����
		{
			if (ioInfo->Mode == READ)
			{
				ChatRoom ChatRoom;
				char cRoomNumberOrName[MAX_CHATROOM_SIZE] = "";
				char CreateCode[10] = "";
				char JoinCode[10] = "";
				char sendMsg_Room[1024] = "";
				char *tmp_request;
				int itmp_RoomNum = 0;
				char ChatLogMsg_tmp[1024] = "";

				if (bytesTrans == 0)    // EOF ���� ��
				{
					CloseClientSock(sock, ioInfo, shareData);
					continue;
				}
				/*ID Ž��*/
				iter = shareData->Clients.begin();
				while (iter != shareData->Clients.end())
				{
					if (iter->hClntSock == sock)
					{
						strcpy(clntName, iter->name);
						sprintf(SendMsg, "[%s] %s", clntName, ioInfo->buffer);
						break;
					}
					else
					{
						iter++;
					}
				}
				if (ioInfo->buffer[0] == '!')//Ư���ɼ� �游���, �� �����ϱ�, �� ������(�濡 �ƹ��� ������ �ڵ����� ������)
				{
					/*
					*���� �����ؾ��� Ŭ�� ��ɾ� �����Ҷ� !create room �̰� ���� ������������
					*!create room alalssk�̸�  !0 alalssk �� �� �����ϵ��� ����
					*
					*/
					//

					//					cout << "��û: " << ioInfo->buffer;
					//sprintf(tmp, "%s %s\n", strtok(ioInfo->buffer," "), strtok(NULL," "));

					if (ioInfo->buffer[1] == '0')//create chat room
					{
						tmp_request = strtok(ioInfo->buffer, " ");
						tmp_request = strtok(NULL, " ");
						strcpy(cRoomNumberOrName, tmp_request);
						cRoomNumberOrName[strlen(cRoomNumberOrName) - 1] = '\0';//Name


						EnterCriticalSection(&cs);
						ChatRoom.ChatRoomNum = ++ChatRoomCount;				//���ȣ
						strcpy(ChatRoom.ClientsID[0], clntName);			//��������ID
						strcpy(ChatRoom.chatRoomName, cRoomNumberOrName);	//ä�ù��̸�
						ChatRoom.UserCount = 1;								//�������� ���� ��
						shareData->ChatRoomList.push_back(ChatRoom);
						LeaveCriticalSection(&cs);
						//cout << clntName << " ���� " << cRoomNumberOrName << "���� �������." << endl;
						//�� ������ ��û�� Ŭ���̾�Ʈ���׸� �����Ϸ� �ڵ带 send��

						sprintf(CreateCode, "!0 %d", ChatRoom.ChatRoomNum);
						send(sock, CreateCode, strlen(CreateCode), 0);
						sprintf(sendMsg_Room, "%s���� %s[%d]���� �������.", clntName, ChatRoom.chatRoomName, ChatRoom.ChatRoomNum);


					}
					else if (ioInfo->buffer[1] == '1')//join chat room
					{
						int input_roomNum = 0;
						bool RoomFindFlag = false;
						tmp_request = strtok(ioInfo->buffer, " ");
						tmp_request = strtok(NULL, " ");
						strcpy(cRoomNumberOrName, tmp_request);
						//cRoomNumberOrName[strlen(cRoomNumberOrName) - 1] = '\0';//Number
						input_roomNum = atoi(cRoomNumberOrName);

						iter_ChatRoom = shareData->ChatRoomList.begin();
						while (iter_ChatRoom != shareData->ChatRoomList.end())//�� ã��
						{
							if (iter_ChatRoom->ChatRoomNum == input_roomNum)
							{
								RoomFindFlag = true;
								break;
							}
							else
							{
								iter_ChatRoom++;
							}
						}
						if (RoomFindFlag)//input �� ��ȣ�� ã������
						{
							if (iter_ChatRoom->UserCount < 3)
							{//���� Ǯ���� �ƴҰ��
								EnterCriticalSection(&cs);
								strcpy(iter_ChatRoom->ClientsID[iter_ChatRoom->UserCount++], clntName);
								LeaveCriticalSection(&cs);
								//�� ������ ��û�� Ŭ���̾�Ʈ ���׸� ������ �Ϸ� �ڵ带 send��
								send(sock, "1", strlen("1"), 0);//TRUE
								sprintf(JoinCode, "!1 %d", input_roomNum);
								send(sock, JoinCode, strlen(JoinCode), 0);
								sprintf(sendMsg_Room, "%s���� %s�� �濡 ������.\n", clntName, input_roomNum);

							}
							else
							{//���� Ǯ���ϰ��(3��)
								cout << clntName << "�� [" << input_roomNum << "] �� �濡 ���� ����(Ǯ��)" << endl;
								send(sock, "0", strlen("0"), 0);//FALSE
								//sprintf(JoinCode, "!1 %d �� Ǯ���Դϴ�.", iter_ChatRoom->ChatRoomNum);
								//send(sock, JoinCode, strlen(JoinCode), 0);
								//Ǯ�� ó���� ���߿�

							}

						}
						else//input ���� �� ã�� ���
						{
							cout << clntName << "�� [" << input_roomNum << "] �� ���� ã�� ���߽��ϴ�." << endl;
							send(sock, "0", strlen("0"), 0);
							//sprintf(JoinCode, "!1 %d ���� ã�� ����", input_roomNum);
							//send(sock, JoinCode, strlen(JoinCode), 0);
							//���� ���� ã������ �޽����� ������ �ʿ䰡�ִ°� �׳� F�� �ѹ� ������ �Ǵ°�
						}

						//���� ����ų� ���� �� �ѹ� ���ȣ�� ����
						//!join 10
						//�濡 �ִ� ��������׵� �޼����� ����������� send�� �ι��ϸ� ���� ¿� �ȴ�
						//!join [���ȣ] [��� ����]
						//ex) !join 10 ID���� �濡 ���� �ϼ̽��ϴ�.
					}//end join
					else if (ioInfo->buffer[1] == '2')//exit chat room
					{
						cout << "�� ������" << endl;
						//send : "!���ȣ(�ɼǺκ�) ID �� ���� �������ϴ�."
					}
					else
					{
						cout << "��ɾ �����ϴ�." << endl;
					}
					//ó�� �� �ϰ� WSARecv()�߰� ������� �����ָ� ���� �޽��� ������
					//mode �� �� if�� �ɸ´� mode��(ex: CREATE_ROOM, EXIT_ROOM ....etc)
					//cout << tmp << "size: " << strlen(tmp) << endl;
					SendMsgFunc(sendMsg_Room, shareData, strlen(sendMsg_Room) + 2);
					cout << sendMsg_Room << endl;
					delete ioInfo;
				}
				else if (ioInfo->buffer[0] == '/')//ä�ø޼��� ���� �ɼ�
				{
					if (ioInfo->buffer[1] == 'w')//�Ӹ�
					{
						cout << "�Ӹ� ������" << endl;
						delete ioInfo;
					}
					else//chat room 0~999
					{
						tmp_request = strtok(ioInfo->buffer, " ");
						itmp_RoomNum = atoi(tmp_request + 1);
						tmp_request = strtok(NULL, " ");

						sprintf(SendMsg, "/%d [%s]%s", itmp_RoomNum, clntName, tmp_request);
						/*blocking Send*/
						SendMsgFunc(SendMsg, shareData, MAX_NAME_SIZE + bytesTrans + 2);//���ȣ Ư�� �ΰ�+


						tmp_request[strlen(tmp_request) - 1] = '\0';//Ŭ�� �������� ���๮�ڰ� ����, ������ ����Ҷ��� �ȵ���
						EnterCriticalSection(&cs);
						//cout << "message received from [" << clntName << ']' << tmp_request << "(" << itmp_RoomNum << ")" << endl;
						sprintf(ChatLogMsg_tmp, "message received from [%s]%s(%d)", clntName, tmp_request, itmp_RoomNum);
						//cout << ChatLogMsg_tmp << endl;
						Chatlog.In_ChatLog_txt(ChatLogMsg_tmp);
						LeaveCriticalSection(&cs);
						delete ioInfo;
						/*WSARecv �Լ�*/
					}

				}
				else
				{
					cout << "Recv Error: " << ioInfo->buffer << endl;
					delete ioInfo;

				}
			}
			else if (ioInfo->Mode == FIRST_READ)//ù name �Է�
			{
				char first_send[20 + 20] = "";

				/**/

				iter = shareData->Clients.begin();
				while (iter != shareData->Clients.end())
				{
					if (iter->hClntSock == sock)
					{
						EnterCriticalSection(&cs);
						strcpy(iter->name, ioInfo->buffer);
						cout << '[' << iter->name << ']' << sock << "�� �̸� �Է� �Ϸ�" << endl;
						LeaveCriticalSection(&cs);
						break;
					}
					else
					{
						iter++;
					}
				}

				/**/
				//shareData->Clients.push_back()
				sprintf(first_send, "%s �� ����\n", ioInfo->buffer);
				cout << first_send;

				SendMsgFunc(first_send, shareData, bytesTrans + 17);
				delete ioInfo;
				//����� ioInfo->Mode = ROOM_READ �� �����ϰ� �� ������ recv send ��
				//Ư�� �ڵ�(����� ������ �����ڵ� ��)�� ������ �׿����� ��� ó��
				/*WSARecv*/
			}
			ioInfo = new OVER_DATA;
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;//������ �����͸� ����....?���⼭ wsaBuf�� �ʿ伺������ �˾ƺ����� ����
			ioInfo->Mode = READ;
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
	//���⼭ �ǹ��� - ũ��Ƽ�û��� ��ü�� ������ �����ص� ����?�� �Ǵ���
	CRITICAL_SECTION cs;
	char CloseName[MAX_NAME_SIZE];
	char tmp[MAX_NAME_SIZE + 128];
	memset(tmp, 0, sizeof(tmp));
	memset(CloseName, 0, sizeof(CloseName));
	InitializeCriticalSection(&cs);
	list<CLIENT_DATA>::iterator iter;
	iter = lpComp->Clients.begin();
	while (iter != lpComp->Clients.end())
	{
		if (iter->hClntSock == sock)
		{
			EnterCriticalSection(&cs);
			strcpy(CloseName, iter->name);
			iter = lpComp->Clients.erase(iter);
			lpComp->Clients_Num--;
			LeaveCriticalSection(&cs);
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
	delete ioInfo;
	cout << tmp;
	//puts("DisConnect Client!");
}