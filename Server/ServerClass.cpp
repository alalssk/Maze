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

	//list<CLIENT_DATA>::iterator iter;
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
				if (sDB.Check_Password(ioInfo->buffer))
				{
					DBcode = 0;//접속ㅇㅋ
					EnterCriticalSection(&cs);

					client_data.hClntSock = sock;
					strcpy(client_data.name, strtok(ioInfo->buffer, "_"));
					client_data.MyRoom = 0;
					shareData->Clients.push_back(client_data);//list
					shareData->Clients_Num++;
					cout << '[' << client_data.name << ']' << client_data.hClntSock << "님이 접속함 - " << endl;
					TotalConnectedClientCount++;
					LeaveCriticalSection(&cs);
				}
				else {
					DBcode = 2;//비번다름코드;
					strcpy(client_data.name, strtok(ioInfo->buffer, "_"));//얘를 cs로 감쌀 필요가 있는지...?
					cout << '[' << client_data.name << "]비번틀림" << endl;

				}
				//

				//

				//shareData->Clients.push_back()
				sprintf(first_send, "@%d", DBcode);
				//SendMsgFunc(first_send, shareData, 5);
				//당연히 해당 소캣에만 보내야되느데 전체send(SnedMsgFunc)를 해가지고 ㅅㅂ
				send(sock, first_send, 5, 0);
				delete ioInfo;
				//여기다 ioInfo->Mode = ROOM_READ 로 설정하고 룸 정보만 recv send 함
				//특정 코드(방생성 방입장 종료코드 등)을 받으면 그에대한 모드 처리

				/*WSARecv*/
				ioInfo = new OVER_DATA;
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				if (DBcode == 2)ioInfo->Mode = FIRST_READ; //비번다르면 다시 읽어야하니까
				else ioInfo->Mode = READ;
			}
			else if (ioInfo->Mode == READ)
			{
				if (ioInfo->buffer[0] == '@')//방생성(@R), 방입장(@J)-MyRoom이 0인 경우만, 방 나가기(@E)-MyRoom이 0이 아닌 경우만
				{
					if (ioInfo->buffer[1] == 'R')					//****** 방 생성 요청 ******
					{												//방 생성 완료 후 모든 클라이언트에 새로운 방들에 대한 정보를 send함
						if (CreateRoomFunc(shareData, sock))//방 생성
						{//방 삭제할때도 보내줘야함
							//그리고 모든클라에 새로운 방정보(!"방개수"_"No.[방번호]>> [방이름]"_... send

							//cout << "방 생성완료" << endl;
							//send(sock, "@R1", 3, 0); -----> CreateRoomFunc 함수내부에서 완료패킷보냄
							SendWaitingRoomList(shareData);
						}
						else
						{
							cout << "방 생성 실패 -> " << sock << endl;
							send(sock, "@R0", 3, 0);
						}


					}
					else if (ioInfo->buffer[1] == 'r')				//****** 방리스트 요청 ******
					{
						send(sock, "@r1", 3, 0);
						SendWaitingRoomList(shareData);
					}
					else if (ioInfo->buffer[1] == 'J')				//****** 방 입장 요청 Join ******
					{//@J_[방번호]
						char *cRoomNum;
						int iRoomNum;
						strtok(ioInfo->buffer, "_");
						cRoomNum = strtok(NULL, "_");
						iRoomNum = atoi(cRoomNum);
						cout << "방 입장 패킷 받음" << iRoomNum << endl;
						if (JoinRoomFunc(shareData, sock, iRoomNum))
						{//방입장성공
							//성공한 경우는 JoinRoomFunc에서 성공패킷 send함
							cout << "방입장성공(@J1) 보냄->" << sock << endl;
						}
						else
						{//방입장 실패
							send(sock, "@J0", 3, 0);
							cout << "방입장실패(@J0) 보냄->" << sock << endl;
						}
						//func(shareData, sock, roomNum)
					}
					else if (ioInfo->buffer[1] == 'E')				//****** 방 나가기 요청 Exit 
					{//@E_[방번호]_[ID]
						cout << "방 나가기 요청 패킷 받음" << endl;
						char *tmp;

						int iRoomNum;
						strtok(ioInfo->buffer, "_");
						tmp = strtok(NULL, "_");
						iRoomNum = atoi(tmp);
						tmp = strtok(NULL, "_");

						if (ExitRoomFunc(shareData, iRoomNum, tmp))
						{
							send(sock, "@E1", 3, 0);

							cout << "방 나가기 요청 완료패킷(@E1) 보냄" << endl;
						}
						else
						{
							send(sock, "@E0", 3, 0);

							cout << "방 나가기 요청 실패패킷(@E0) 보냄" << endl;
						}


					}
					else if (ioInfo->buffer[1] == 'L')				//****** 게임 로그아웃요청 ******
					{//
						cout << "로그아웃 요청 패킷 받음 -> " << sock;
						/*로그아웃 처리부분 깔끔한 로그아웃과 게임종료를 위해 해주면 좋지만 일단 지금은 안함*/

						send(sock, "@L1", 3, 0);
						cout << "--->(@L1)전송 완료" << endl;
						CloseClientSock(sock, ioInfo, shareData);

					}
					else if (ioInfo->buffer[1] == 'G')			//****** 게임종료요청 ******
					{//
						cout << "게임종료 요청 패킷 받음 -> " << sock;
						/*로그아웃 처리부분 깔끔한 로그아웃과 게임종료를 위해 해주면 좋지만 일단 지금은 안함*/
						send(sock, "@G1", 3, 0);
						cout << "--->(@G1)전송 완료" << endl;
						CloseClientSock(sock, ioInfo, shareData);
					}
				}//end @
				else if (ioInfo->buffer[0] == '/')
				{	// recv " /[방번호]_[Id]_[내용] "
					//func(int 방번호, char* [ID]_[내용]) ->> 아이디랑 내용은 클라에서 나누니까 그냥 통째로 보내면 됨.


					int RoomNum;
					char *tmp;
					tmp = strtok(ioInfo->buffer + 1, "_");
					RoomNum = atoi(tmp);
					cout << "======== WaitingRoom[" << RoomNum << "]Chat Send ========" << endl;
					tmp = strtok(NULL, "");
					SendMsgWaitingRoomFunc(RoomNum, shareData, tmp);
					cout << "======== WaitingRoom[" << RoomNum << "]Chat Send ========" << endl;
					// send " /1_ID_내용 " 여기서 1은 방번호가 아니라 send성공을 의미함
				}
				else if (ioInfo->buffer[0] == '$')
				{
					cout << "게임시작 패킷(" << ioInfo->buffer << ")받음 " << endl;
					send(sock, "$1_", 3, 0);
				}
				/*WSARecv*/
				delete ioInfo;
				ioInfo = new OVER_DATA;
				memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
				ioInfo->Mode = READ;

			}

			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;//버퍼의 포인터를 담음....?여기서 wsaBuf의 필요성에대해 알아보도록 하자
			WSARecv(sock, &(ioInfo->wsaBuf),
				1, NULL, &flags, &(ioInfo->overlapped), NULL);
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
					cout << "소캣(" << sock << ')' << " 연결 해제됨: ";
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

				sprintf(tmpRoomName, "[%s]님의 방입니다.", iter->name);
				strcpy(room.chatRoomName, tmpRoomName);
				room.ChatRoomNum = TotalCreateRoomCount + 1;
				strcpy(room.ClientsID[0], iter->name);
				memset(room.ClientsID[1], 0, sizeof(room.ClientsID[1]));
				memset(room.ClientsID[2], 0, sizeof(room.ClientsID[2]));
				room.hClntSock[0] = sock;//만드는애도 소캣 넣어줘야지!!
				room.UserCount = 1;

				iter->MyRoom = room.ChatRoomNum;

				EnterCriticalSection(&cs);//cs
				lpComp->ChatRoomList.push_back(room);
				TotalCreateRoomCount++;
				LeaveCriticalSection(&cs);//cs
				//방 만든놈한테만 방정보 전송

				sprintf(CreateRoomSendMsg, "@R1_%d_%s", room.ChatRoomNum, room.chatRoomName);
				send(sock, CreateRoomSendMsg, strlen(CreateRoomSendMsg), 0);
				cout << "방 생성완료>>";
				cout << CreateRoomSendMsg << "전송 성공" << endl;
				//============================유저리스트 전송부분============================
				/*이부분은 클라에서 처음 방 만들때 ID등 자기 정보 방에 저장하기 떄문에 굳ㅇ ㅣ보내줄 필요없는듯 나중에 확인해보고 지우자*/
				memset(CreateRoomSendMsg, 0, sizeof(CreateRoomSendMsg));
				cout << "================유저리스트 전송부================" << endl;
				cout << CreateRoomSendMsg << endl;
				sprintf(CreateRoomSendMsg, "@U1_%s-10_", iter->name);
				cout << CreateRoomSendMsg << " >> 전송" << endl;
				send(sock, CreateRoomSendMsg, strlen(CreateRoomSendMsg), 0);
				//============================유저리스트 전송부분============================
				return true;
			}
			else
			{
				return false;//이미 방에 입장중
			}
		}
		else
		{
			iter++;
		}
	}
	return false;//ID를 찾을수 없음.


}
const bool ServerClass::ExitRoomFunc(LPShared_DATA lpComp, int RoomNum, char *id)
{//이 함수는 항상 cs안에있어야함
	list<ChatRoom>::iterator iter_room;
	list<CLIENT_DATA>::iterator iter_user;
	iter_user = lpComp->Clients.begin();
	while (iter_user != lpComp->Clients.end())
	{
		if (strcmp(iter_user->name, id) == 0)
		{
			cout << id << " :검색 완료: " << iter_user->name << endl;
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
			{//방에 나뿐이없으면 그냥 방 삭제
				iter_room = lpComp->ChatRoomList.erase(iter_room);
				//방이 없어졌으니 새로운 방 정보들을 모든 클라에 send
				iter_user->MyRoom = 0;
				SendWaitingRoomList(lpComp);
				return true;
			}
			else
			{//방에 나말고 다른사람도 있으면....그냥 나만 나가
				for (int i = 0; i < 3; i++)
				{//대기방 ID리스트에서 해당ID 지우는 과정
					if (strcmp(iter_room->ClientsID[i], id) == 0)
					{
						memset(iter_room->ClientsID[i], 0, sizeof(iter_room->ClientsID[i]));
						for (int j = i; j < 3; j++)
						{//여기다 소캣도 추가해줘
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
				//============================유저리스트 전송부분============================
				char ExitRoomSendMsg[100] = "";
				int MsgSz;
				memset(ExitRoomSendMsg, 0, sizeof(ExitRoomSendMsg));
				for (int i = 0; i < iter_room->UserCount; i++)
				{//@U1_alalssk-5_test-10_
					if (i == 0)strcpy(ExitRoomSendMsg, "@U1_");
					else strcat(ExitRoomSendMsg, "_");
					strcat(ExitRoomSendMsg, iter_room->ClientsID[i]);
					strcat(ExitRoomSendMsg, "-");
					strcat(ExitRoomSendMsg, "10");//임시winCount
				}
				strcat(ExitRoomSendMsg, "_");

				MsgSz = strlen(ExitRoomSendMsg);
				for (int i = 0; i < iter_room->UserCount; i++)
				{
					cout << '[' << iter_room->hClntSock[i] << "] 방나가기 유저리스트 전송 >> " << ExitRoomSendMsg << endl;
					//유저카운트만 마이너스되고 방유저 배열이 안바뀐듯
					send(iter_room->hClntSock[i], ExitRoomSendMsg, MsgSz, 0);
				}
				//============================유저리스트 전송부분============================
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
	{//방을 못찾았으면
		cout << "방을 찾지 못함" << endl;
		return false;
	}
	else if (iter_room->UserCount >= 3)
	{//방을 찾았지만 꽉찬경우
		cout << "방이 꽉참" << endl;
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
				cout << "방 입장완료 >> ";
				cout << JoinRoomSendMsg << "전송 성공" << endl;

				//====================유저리스트 전송부분============================
				memset(JoinRoomSendMsg, 0, sizeof(JoinRoomSendMsg));
				for (int i = 0; i < iter_room->UserCount; i++)
				{//@U1_alalssk-5_test-10_
					if (i == 0)strcpy(JoinRoomSendMsg, "@U1_");
					else strcat(JoinRoomSendMsg, "_");
					strcat(JoinRoomSendMsg, iter_room->ClientsID[i]);
					strcat(JoinRoomSendMsg, "-");
					strcat(JoinRoomSendMsg, "10");//임시winCount
				}
				strcat(JoinRoomSendMsg, "_");

				MsgSz = strlen(JoinRoomSendMsg);
				for (int i = 0; i < iter_room->UserCount; i++)
				{
					cout << '[' << iter_room->hClntSock[i] << "]조인 유저리스트 전송 >> " << JoinRoomSendMsg << endl;
					send(iter_room->hClntSock[i], JoinRoomSendMsg, MsgSz, 0);
				}
				//====================유저리스트 전송부분============================
				return true;
			}
			else iter_user++;
		}
		if (iter_user == lpComp->Clients.end())
		{
			cout << "접속죽인 유저가 아닙니다" << endl;
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
	cout << shareData->ChatRoomList.size() << "개의 대기방" << endl;
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
		_itoa(iter->ChatRoomNum, cRoomNum, 10);//itoa( 숫자값 , 값이 들어갈 string 배열 , 숫자값을 변환할 진수 ) 
		strcat(SendWaitingRoomListBuf, cRoomNum);
		strcat(SendWaitingRoomListBuf, ">>");
		strcat(SendWaitingRoomListBuf, iter->chatRoomName);
		iter++;
	}
	cout << SendWaitingRoomListBuf;
	SendMsgFunc(SendWaitingRoomListBuf, lpComp, strlen(SendWaitingRoomListBuf));
	cout << "::방 리스트 보내기 완료" << endl;
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
		cout << iter_room->UserCount << " 명 접속중" << endl;
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