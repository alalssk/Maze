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
	//memset(lpComPort, 0, sizeof(ComPort)); 왜 이렇게 초기화하면 push_back 부분에서 크래쉬가 나는지...?
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
	/*크리티컬색션 객체를 루틴마다 생성해서 해야하나...?
	한개만 생성해서 해야하나*/
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

		EnterCriticalSection(&cs);
		cout << "Accept 처리중..." << endl;

		lpComPort->Clients.push_back(client_data);//list
		lpComPort->Clients_Num++;
		TotalConnectedClientCount++;
		LeaveCriticalSection(&cs);
		if (CreateIoCompletionPort(
			(HANDLE)client_data.hClntSock,
			lpComPort->hComPort,
			(DWORD)client_data.hClntSock,
			0
			) == NULL)//컴플리션키로 클라소캣을 넣었음
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
		iter++;//ㅅㅂ뺴먹지 말자
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
			);//errorㅊ리 꼭 해줘야함 
		if (bGQCS)//gqcs 성공
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

				if (bytesTrans == 0)    // EOF 전송 시
				{
					CloseClientSock(sock, ioInfo, shareData);
					continue;
				}
				/*ID 탐색*/
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
				if (ioInfo->buffer[0] == '!')//특수옵션 방만들기, 방 입장하기, 방 나가기(방에 아무도 없으면 자동으로 방제거)
				{
					/*
					*완전 수정해야함 클라서 명령어 전송할때 !create room 이거 고대로 전송하지말고
					*!create room alalssk이면  !0 alalssk 만 딱 전송하도록 변경
					*
					*/
					//

					//					cout << "요청: " << ioInfo->buffer;
					//sprintf(tmp, "%s %s\n", strtok(ioInfo->buffer," "), strtok(NULL," "));

					if (ioInfo->buffer[1] == '0')//create chat room
					{
						tmp_request = strtok(ioInfo->buffer, " ");
						tmp_request = strtok(NULL, " ");
						strcpy(cRoomNumberOrName, tmp_request);
						cRoomNumberOrName[strlen(cRoomNumberOrName) - 1] = '\0';//Name


						EnterCriticalSection(&cs);
						ChatRoom.ChatRoomNum = ++ChatRoomCount;				//방번호
						strcpy(ChatRoom.ClientsID[0], clntName);			//입장중인ID
						strcpy(ChatRoom.chatRoomName, cRoomNumberOrName);	//채팅방이름
						ChatRoom.UserCount = 1;								//입장중인 유저 수
						shareData->ChatRoomList.push_back(ChatRoom);
						LeaveCriticalSection(&cs);
						//cout << clntName << " 님이 " << cRoomNumberOrName << "방을 만들었음." << endl;
						//방 생성을 요청한 클라이언트한테만 생성완료 코드를 send함

						sprintf(CreateCode, "!0 %d", ChatRoom.ChatRoomNum);
						send(sock, CreateCode, strlen(CreateCode), 0);
						sprintf(sendMsg_Room, "%s님이 %s[%d]방을 만들었음.", clntName, ChatRoom.chatRoomName, ChatRoom.ChatRoomNum);


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
						while (iter_ChatRoom != shareData->ChatRoomList.end())//방 찾기
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
						if (RoomFindFlag)//input 방 번호를 찾았으면
						{
							if (iter_ChatRoom->UserCount < 3)
							{//방이 풀방이 아닐경우
								EnterCriticalSection(&cs);
								strcpy(iter_ChatRoom->ClientsID[iter_ChatRoom->UserCount++], clntName);
								LeaveCriticalSection(&cs);
								//방 입장을 요청한 클라이언트 한테만 방입장 완료 코드를 send함
								send(sock, "1", strlen("1"), 0);//TRUE
								sprintf(JoinCode, "!1 %d", input_roomNum);
								send(sock, JoinCode, strlen(JoinCode), 0);
								sprintf(sendMsg_Room, "%s님이 %s번 방에 입장함.\n", clntName, input_roomNum);

							}
							else
							{//방이 풀방일경우(3명)
								cout << clntName << "가 [" << input_roomNum << "] 번 방에 입장 실패(풀방)" << endl;
								send(sock, "0", strlen("0"), 0);//FALSE
								//sprintf(JoinCode, "!1 %d 은 풀방입니다.", iter_ChatRoom->ChatRoomNum);
								//send(sock, JoinCode, strlen(JoinCode), 0);
								//풀방 처리는 나중에

							}

						}
						else//input 방을 못 찾은 경우
						{
							cout << clntName << "가 [" << input_roomNum << "] 번 방을 찾지 못했습니다." << endl;
							send(sock, "0", strlen("0"), 0);
							//sprintf(JoinCode, "!1 %d 방을 찾지 못함", input_roomNum);
							//send(sock, JoinCode, strlen(JoinCode), 0);
							//굳이 방을 찾지못한 메시지를 보내줄 필요가있는가 그냥 F만 한번 보내면 되는걸
						}

						//방을 만들거나 들어갈때 딱 한번 방번호를 전송
						//!join 10
						//방에 있는 사람들한테도 메세지를 전송해줘야함 send를 두번하면 낭비 쩔어서 안댐
						//!join [방번호] [출력 내용]
						//ex) !join 10 ID님이 방에 입장 하셨습니다.
					}//end join
					else if (ioInfo->buffer[1] == '2')//exit chat room
					{
						cout << "방 나가기" << endl;
						//send : "!방번호(옵션부분) ID 가 방을 나갔습니다."
					}
					else
					{
						cout << "명령어가 없습니다." << endl;
					}
					//처리 다 하고 WSARecv()추가 해줘야함 안해주면 다음 메시지 못받음
					//mode 는 각 if에 걸맞는 mode로(ex: CREATE_ROOM, EXIT_ROOM ....etc)
					//cout << tmp << "size: " << strlen(tmp) << endl;
					SendMsgFunc(sendMsg_Room, shareData, strlen(sendMsg_Room) + 2);
					cout << sendMsg_Room << endl;
					delete ioInfo;
				}
				else if (ioInfo->buffer[0] == '/')//채팅메세지 전송 옵션
				{
					if (ioInfo->buffer[1] == 'w')//귓말
					{
						cout << "귓말 구현부" << endl;
						delete ioInfo;
					}
					else//chat room 0~999
					{
						tmp_request = strtok(ioInfo->buffer, " ");
						itmp_RoomNum = atoi(tmp_request + 1);
						tmp_request = strtok(NULL, " ");

						sprintf(SendMsg, "/%d [%s]%s", itmp_RoomNum, clntName, tmp_request);
						/*blocking Send*/
						SendMsgFunc(SendMsg, shareData, MAX_NAME_SIZE + bytesTrans + 2);//대괄호 특문 두개+


						tmp_request[strlen(tmp_request) - 1] = '\0';//클라에 보낼떄는 개행문자가 들어가게, 서버에 출력할때는 안들어가게
						EnterCriticalSection(&cs);
						//cout << "message received from [" << clntName << ']' << tmp_request << "(" << itmp_RoomNum << ")" << endl;
						sprintf(ChatLogMsg_tmp, "message received from [%s]%s(%d)", clntName, tmp_request, itmp_RoomNum);
						//cout << ChatLogMsg_tmp << endl;
						Chatlog.In_ChatLog_txt(ChatLogMsg_tmp);
						LeaveCriticalSection(&cs);
						delete ioInfo;
						/*WSARecv 함수*/
					}

				}
				else
				{
					cout << "Recv Error: " << ioInfo->buffer << endl;
					delete ioInfo;

				}
			}
			else if (ioInfo->Mode == FIRST_READ)//첫 name 입력
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
						cout << '[' << iter->name << ']' << sock << "의 이름 입력 완료" << endl;
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
				sprintf(first_send, "%s 님 접속\n", ioInfo->buffer);
				cout << first_send;

				SendMsgFunc(first_send, shareData, bytesTrans + 17);
				delete ioInfo;
				//여기다 ioInfo->Mode = ROOM_READ 로 설정하고 룸 정보만 recv send 함
				//특정 코드(방생성 방입장 종료코드 등)을 받으면 그에대한 모드 처리
				/*WSARecv*/
			}
			ioInfo = new OVER_DATA;
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;//버퍼의 포인터를 담음....?여기서 wsaBuf의 필요성에대해 알아보도록 하자
			ioInfo->Mode = READ;
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
	//여기서 의문점 - 크리티컬색션 객체는 여러개 생성해도 연동?이 되는지
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