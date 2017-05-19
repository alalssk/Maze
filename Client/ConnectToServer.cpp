#include "ConnectToServer.h"


ConnectToServer::ConnectToServer()
{
}


ConnectToServer::~ConnectToServer()
{
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

	return true;
}

bool ConnectToServer::StartConnect(char* ID_Pass)
{
	int strLen;
	char code[CONNECT_CODE_SIZE] = "";
	if (connect(Sock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		fputs("connect() error!", stderr);
		fputc('\n', stderr);
		return false;
	}
	/*이 부분에서 ID_Pass를 서버로 한 번 정송하여 DB 처리 후 Connect완료*/
	send(Sock, ID_Pass, strlen(ID_Pass)+1, 0);
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
				return true;
				break;
			case '1':
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