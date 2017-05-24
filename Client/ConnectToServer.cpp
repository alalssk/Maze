#include "ConnectToServer.h"
bool ConnectToServer::ExitFlag = false;
bool ConnectToServer::LogoutFlag = false;
int ConnectToServer::ClientMode = 0;
bool ConnectToServer::threadOn=false;//�ӽ�
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

	/*�� �κп��� ID_Pass�� ������ �� �� �����Ͽ� DB ó�� �� ���� �Ϸ� ó��*/
	send(Sock, ID_Pass, strlen(ID_Pass) + 1, 0);
	//���ӿϷ�(@0), ���ӽ���{���̵��ߺ�(@1), ��й�ȣ�ٸ�(@2) ��}
	//DB���� ��Ŷ�� @[code] (@0, @1, @2)
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
			case '1'://�ϴ� �Ⱦ� 
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
	//ClientMode >= 1(lobby����) �϶� �Ѿ���� �̺�Ʈ ó�� WaitSingleObject(hEvent,INFINITE);
	//�׷� �α׾ƿ��Ҷ� �����嵵 ���������߰���
	while (!threadOn)
	{
		Sleep(500);
	}
	while (!ExitFlag)
	{
		strLen = recv(td.sock, recvMsg, BUF_SIZE - 1, 0);	//�� �κп� send�����忡�� �����÷��װ� ������ �̺�Ʈ ó����....
		//���2. ��Ĺ�� �ͺ��ŷ���� ����� (ioctlsocket)
		if (strLen == -1)
			return -1;

		if (recvMsg[0] == '!')
		{
			//������ �޾ƿ�==> !"�氳��"_"No.[���ȣ]>> [���̸�]"_...
			//!0 �̸� ���̾��ٴ� ����
			//�ϴ� �������� ! ��� ������ ������ �Լ��� ����� �����ϴ��� üũ����
			if (recvMsg[1] == '0')td.lobby.PrintWaitionRoomList("0");
			else td.lobby.PrintWaitionRoomList(recvMsg + 1);
		}
		else if (recvMsg[0] == '/')
		{//ä�ø޽��� ����

		}
		else { cout << "�������� �޼���: " << recvMsg << endl; }
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