#include "ServerClass.h"
int ServerClass::TotalConnectedClientCount = 0;
bool ServerClass::ExitFlag = false;
int ServerClass::ChatRoomCount = 0;
LogClass ServerClass::Chatlog;
LogClass ServerClass::DBLog;
ServerDB ServerClass::sDB;
CRITICAL_SECTION ServerClass::cs;
ServerClass::ServerClass()
{
	shareData = new Shared_DATA;
	//memset(lpComPort, 0, sizeof(ComPort)); 왜 이렇게 초기화하면 push_back 부분에서 크래쉬가 나는지...?
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
	cout << TotalConnectedClientCount << "명 접속중" << endl;
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
		printf("%d명 접속중....", lpComPort->Clients_Num);
		printf("%d 번 째 클라이언트 Accept 대기중\n", TotalConnectedClientCount);
		client_data.hClntSock = accept(
			lpComPort->hServSock,
			(SOCKADDR*)&client_data.clntAdr,
			&addrLen
			);
		/*깔끔한 종료처리를 위해서는 비동기 accept를 해야한다 어떻게?*/
		/*굳이 비동기 처리를 할 필요가있나...? 서버 종료할때 accept스레드를 종료키시면 되지않나..?*/
		cout << "Accept 처리중..." << endl;
		//
		if (CreateIoCompletionPort(
			(HANDLE)client_data.hClntSock,
			lpComPort->hComPort,
			(DWORD)client_data.hClntSock,//컴플리션키로 
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
		ioInfo->Mode = FIRST_READ;//첫 접속 처리를 위한 모드(ID password 처리)
		/*IO_PENDING 에러처리*/
		cout << "client sock: " << client_data.hClntSock<<'-' << inet_ntoa(client_data.clntAdr.sin_addr) << endl;
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
		iter++;//ㅅㅂ뺴먹지 말자
	}
}
unsigned  __stdcall ServerClass::IOCPWorkerThread(LPVOID CompletionPortIO)
{
	//HANDLE hComPort = (HANDLE)pComPort;
	LPShared_DATA shareData = (LPShared_DATA)CompletionPortIO;

	/*컴플리션키*/
	SOCKET sock;
	/*컴플리션키*/

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
			&sock,//컴플리션키
			(LPOVERLAPPED*)&ioInfo,
			INFINITE
			);//errorㅊ리 꼭 해줘야함 
		if (bGQCS)//gqcs 성공
		{

			if (ioInfo->Mode == FIRST_READ)//ID_PASS 입력된걸 DB처리
			{
				char first_send[5] = "";
				int DBcode;
				cout << ioInfo->buffer << endl;
				if (sDB.Check_Password(ioInfo->buffer))
				{
					DBcode = 0;//접속ㅇㅋ
					EnterCriticalSection(&cs);

					client_data.hClntSock = sock;
					strcpy(client_data.name, strtok(ioInfo->buffer, "_"));

					shareData->Clients.push_back(client_data);//list
					shareData->Clients_Num++;
					cout << '[' << client_data.name << ']' << client_data.hClntSock << "님이 접속함 - " << endl;
					TotalConnectedClientCount++;
					LeaveCriticalSection(&cs);
				}
				else {
					DBcode = 2;//비번다름;
					cout << '[' << sock << "]비번다름" << endl;

				}
				//

				//

				//shareData->Clients.push_back()
				sprintf(first_send, "@%d", DBcode);
				//SendMsgFunc(first_send, shareData, 5);
				//당연히 해당 소캣에만 보내야되느데 전체send(SnedMsgFunc)를 해가지고 ㅅㅂ
				send(sock, first_send, 5, 0);
				cout << first_send << "전송완료" << endl;
				delete ioInfo;
				//여기다 ioInfo->Mode = ROOM_READ 로 설정하고 룸 정보만 recv send 함
				//특정 코드(방생성 방입장 종료코드 등)을 받으면 그에대한 모드 처리
				/*WSARecv*/
				ioInfo = new OVER_DATA;
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				ioInfo->wsaBuf.len = BUF_SIZE;
				ioInfo->wsaBuf.buf = ioInfo->buffer;//버퍼의 포인터를 담음....?여기서 wsaBuf의 필요성에대해 알아보도록 하자
				if (DBcode == 2)ioInfo->Mode = FIRST_READ; //비번다르면 다시 읽어야하니까
				else ioInfo->Mode = READ;
				WSARecv(sock, &(ioInfo->wsaBuf),
					1, NULL, &flags, &(ioInfo->overlapped), NULL);
			}
			else if (ioInfo->Mode == ROOM_READ)
			{
				cout << "room READ 부분" << endl;
			}

		}
		else
		{
			/*gqcs 실패
			* - 1. GQCS의 4번째인자(inInfo) 가 NULL 인 경우 -
			* 2,3번 인자 역시 정의되지 않는다.
			* 함수 자체 에러로, 타임아웃발생(WAIT_TIMEOUT) 이나 IOCP핸들이 닫힌 경우(ERROR_ABANDONED_WAIT_0)인 경우 발생
			*
			* - 2. GQCS의 4번째 인자(inInfo) 가 NULL이 아닌 경우 -
			* IOCP와 연결된 장치의 입출력돠정에 에러가 발생한 경우임
			* iocp큐로부터 해당 입출력 완료 패킷이 dequeue된 상태다. 따라서 2,3번 매개변수 모두 적절한 값으로 채워짐.
			* ipOverlapped의 Internal 필드는 장치의 상태코드를 담고 있음.
			* GetLastError를 호출하면 해당 상태코드에 매치되는 win32 에러코드를 획득할 수 있음.
			***** NULL이 아닌 경우에는 해당 장치 핸들에 대한 에러처리만 수행 후 IOCP에 연결된 다른 장치의 입출력을 받아들이기 위해 계속 루프를 돌려야 햔다. *****
			*/
			DWORD dwErrCode = GetLastError();
			if (ioInfo != NULL)
			{
				switch (dwErrCode)
				{
				case ERROR_NETNAME_DELETED:
					cout << "소캣 연결 해제됨: ";
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
				{//지정한 시간 경과, INFINITE로 설정해놨기 때문에 굳이 정의할 필요없음.
					cout << "GQCS Error: Time out(" << dwErrCode << ')' << endl;
					break;
				}
				else
				{//gqcs 호출자체 문제 (IOCP핸들이 닫힌 경우)
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
	//SOCKADDR_IN servAdr;//얘는 바인드까지만 쓰고 안쓰기 때문에? 생성하고 없어져도됨?
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
	TotalConnectedClientCount--;
	sprintf(tmp, "[%s] is disconnected...\n", CloseName);
	SendMsgFunc(tmp, lpComp, strlen(tmp));
	delete ioInfo;
	cout << tmp;
	//puts("DisConnect Client!");
}