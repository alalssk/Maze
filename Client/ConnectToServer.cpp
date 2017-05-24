#include "ConnectToServer.h"
bool ConnectToServer::ExitFlag = false;
bool ConnectToServer::LogoutFlag = false;
int ConnectToServer::ClientMode = 0;
bool ConnectToServer::threadOn=false;//임시
ConnectToServer::ConnectToServer()// :ExitFlag(false), LogoutFlag(false)
{
}
ConnectToServer::~ConnectToServer()
{
	WSACleanup();
	closesocket(Sock);
}

void ConnectToServer::CleanupSock()
{
	WSACleanup();
	closesocket(Sock);
}
bool ConnectToServer::setupSock()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		fputs("WSAStartup() error!", stderr);
		fputc('\n', stderr);
		return false;
	}
	Sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(9191);
	if (connect(Sock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		fputs("connect() error!", stderr);
		fputc('\n', stderr);
		return false;
	}
	td.sock = Sock;
	hRcvThread = (HANDLE)_beginthreadex(NULL, 0, &RecvMsg, (void*)&td, 0, NULL);


	return true;
}

bool ConnectToServer::StartConnect(char* ID_Pass)
{
	int strLen;
	char code[CONNECT_CODE_SIZE] = "";

	/*이 부분에서 ID_Pass를 서버로 한 번 정송하여 DB 처리 후 접속 완료 처리*/
	send(Sock, ID_Pass, strlen(ID_Pass) + 1, 0);
	//접속완료(@0), 접속실패{아이디중복(@1), 비밀번호다름(@2) 등}
	//DB관련 패킷은 @[code] (@0, @1, @2)
	strLen = recv(Sock, code, CONNECT_CODE_SIZE - 1, 0);
	if (strLen == -1)
	{
		cout << "Recv Error(ConnectToServer.cpp)" << endl;
		return false;
	}
	else
	{
		if (code[0] == '@')
		{
			switch (code[1])
			{
			case '0':
				threadOn = true;
				return true;
				break;
			case '1'://일단 안씀 
				return false;
				break;
			case '2':
				return false;
				break;
			}
		}
	}

	return false;//
}
const SOCKET ConnectToServer::getSocket()
{
	return this->Sock;
}

unsigned WINAPI ConnectToServer::RecvMsg(void * arg)   // read thread main
{
	ThreadData td = *((ThreadData*)arg);
	char recvMsg[BUF_SIZE]="";
	char *tmp_tok;
	int itmp_RoomNum;
	int strLen;
	//ClientMode >= 1(lobby상태) 일때 넘어가도록 이벤트 처리 WaitSingleObject(hEvent,INFINITE);
	//그럼 로그아웃할때 스레드도 종료시켜줘야겠지
	while (!threadOn)
	{
		Sleep(500);
	}
	while (!ExitFlag)
	{
		strLen = recv(td.sock, recvMsg, BUF_SIZE - 1, 0);	//이 부분에 send스레드에서 종료플레그가 켜지면 이벤트 처리를....
		//방법2. 소캣을 넌블로킹으로 만들기 (ioctlsocket)
		if (strLen == -1)
			return -1;

		if (recvMsg[0] == '!')
		{
			//방정보 받아옴==> !"방개수"_"No.[방번호]>> [방이름]"_...
			//!0 이면 방이없다는 말임
			//일단 서버에서 ! 요고만 보내고 방정보 함수가 제대로 동작하는지 체크하자
			if (recvMsg[1] == '0')td.lobby.PrintWaitionRoomList("0");
			else td.lobby.PrintWaitionRoomList(recvMsg + 1);
		}
		else if (recvMsg[0] == '/')
		{//채팅메시지 전용

		}
		else { cout << "옳지않은 메세지: " << recvMsg << endl; }
	}
	cout << "exitRecvMsg" << endl;
	return 0;
}


//
//void ConnectToServer::SetExitFlag(bool flag)
//{
//	this->ExitFlag = flag;
//}
//void ConnectToServer::SetLogoutFlag(bool flag)
//{
//	this->LogoutFlag = flag;
//}
//bool ConnectToServer::GetExitFlag()
//{
//	return this->ExitFlag;
//}
//bool ConnectToServer::GetLogoutFlag()
//{
//	return this->LogoutFlag;
//}