#include "Lobby.h"
int Lobby::ClientMode = 0;
bool Lobby::threadOn = false;//임시
bool Lobby::ExitFlag = false;
bool Lobby::LogoutFlag = false;
HANDLE Lobby::hEventForRequest;

Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}
const int Lobby::LobbyMain(SOCKET sock)
{
	setSock(sock);
	td.sock = sock;
	td.lobby = this;
	hEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	hRcvThread = (HANDLE)_beginthreadex(NULL, 0, &RecvMsg, (void*)&td, 0, NULL);

	ClearXY();
	PrintLobbyListBox();
	GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
	PrintLobbyTxt();
	while (1)
	{
		key = getKeyDirectionCheck();
		Linfo.SetLobbyFlag(key);

		if (Linfo.GetLobbyFlag() == false)
		{

			Linfo.SetLobbyTxtNum(key);
			initRoomListCheck();
			AllClearPrintLobbyTxtBox();
			GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
			PrintLobbyTxt();
		}
		else
		{

			//RoomInfoListMain();
			Linfo.SetLobbyListPointNumber(key);
			initRoomListCheck();
			PrintLobbyListCheck(Linfo.GetLobbyListPointNumber());

			AllClearPrintLobbyTxtBox();
			PrintLobbyTxt();
		}



		if (Linfo.GetLobbyTxtNum() == 0) //방만들기
		{
			if (key == SPACE || key == ENTER)
			{//[방번호]########의 방
				if(req_CreateRoom())return 0;
				else
				{
					for (int i = 0; i < 3; i++)
					{
						gotoxy(40, 15);
						cout << "###########방생성 실패#############";
						Sleep(500);
						gotoxy(40, 15);
						cout << "                                   ";
						Sleep(500);
					}
					continue;
				}
			}
		}
		//else if (Linfo.GetLobbyTxtNum() == 3)
		//{
		//	if (key == SPACE || key == ENTER)
		//	{
		//		//ClearXY();

		//		return 3; //대기방참가(join)
		//	}
		//} 메뉴 지웠음 방 선택으로 입장 가능하게 할거니깐!
		//그러면 얘를 3으로 해야지 ㅅㅂ 안그러면 lobbyTxtNum 이 3이안되잖아
		else if (Linfo.GetLobbyTxtNum() == 1) //logoout
		{
			if (key == SPACE || key == ENTER)
			{
				if (req_LogoutClient())
				{
					LogoutFlag = true;
					return 4;

				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						gotoxy(40, 15);
						cout << "###########로그아웃 실패#############";
						Sleep(500);
						gotoxy(40, 15);
						cout << "                                     ";
						Sleep(500);
					}
					continue;
				}
			}
		}
		else if (Linfo.GetLobbyTxtNum() == 2)// exit
		{
			if (key == SPACE || key == ENTER)
			{
				if (req_ExitClient())
				{
					ExitFlag = true;
					return 2;

				}
				else
				{
					for (int i = 0; i < 3; i++)
					{
						gotoxy(40, 15);
						cout << "###########게임종료 실패#############";
						Sleep(500);
						gotoxy(40, 15);
						cout << "                                     ";
						Sleep(500);
					}
					continue;
				}

			}
		}
	}

}
void Lobby::PrintLobbyListBox()
{
	int i = 0;
	gotoxy(16, 3);
	cout << " 대 기 방 목 록" << endl;
	GrideBox(5, 4, 14, 18);
}
void Lobby::AllClearPrintLobbyTxtBox()
{
	for (int i = 0; i < 12; i++)
	{
		gotoxy(46, 6 + i);
		cout << "                ";
	}
}
void Lobby::PrintLobbyTxt()
{
	for (int i = 0; i < LOBBY_TXT_NUM_MAX; i++)
	{
		gotoxy(48, 7 + i * 3);
		switch (i)
		{
		case 0:
			cout << "방 만 들 기"; break;
		case 1:
			cout << "로 그 아 웃"; break;
		case 2:
			cout << "게 임 종 료"; break;
		}
	}
}

void Lobby::initRoomListCheck()
{
	for (int i = 0; i < 14; i++)
	{
		gotoxy(7, 5 + i);
		cout << "   ";
	}

}

void Lobby::setSock(SOCKET sock)
{
	this->sock = sock;
}
bool Lobby::req_CreateRoom()
{
	send(sock, "@R", 2, 0);
	cout << "방생성요청보냄" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest,3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_LogoutClient()
{
	send(sock, "@L", 2, 0);
	cout << "로그아웃요청보냄" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_ExitClient()
{
	send(sock, "@G", 2, 0);
	cout << "게임종료요청보냄" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
void Lobby::PrintWaitionRoomList(char *buf)
{//[방개수]_[No.[방번호] 방이름]_[방이름]_[방이름]
	//WaitingRoomCount
	char *tmp;
	int iRoomCount;
	initPrintWaitingRoomList();
	if (buf[0] == '0')
	{//방이없음
		Linfo.WaitingRoomCount = 0;
	}
	else
	{//방이 있는경우

		tmp = strtok(buf, "_");
		iRoomCount = atoi(tmp);
		Linfo.WaitingRoomCount = iRoomCount;
		for (int i = 0; i < Linfo.WaitingRoomCount - 1; i++)
		{
			gotoxy(7 + 3, 5 + i);
			tmp = strtok(NULL, "_");
			cout << tmp << endl;
		}
	}


}

void Lobby::initPrintWaitingRoomList()
{
	for (int i = 0; i < 14; i++)
	{
		gotoxy(7 + 3, 5 + i);
		cout << "                    ";
	}
}

void Lobby::PrintLobbyListCheck(int WaitingRoomListPointNumber)
{
	gotoxy(7, 5 + WaitingRoomListPointNumber);
	cout << "☞ ";
}


unsigned WINAPI Lobby::RecvMsg(void * arg)   // read thread main
{
	ThreadData td = *((ThreadData*)arg);
	char recvMsg[BUF_SIZE] = "";
	char *tmp_tok;
	int itmp_RoomNum;
	int strLen;
	//ClientMode >= 1(lobby상태) 일때 넘어가도록 이벤트 처리 WaitSingleObject(hEvent,INFINITE);
	//그럼 로그아웃할때 스레드도 종료시켜줘야겠지

	while (!ExitFlag && !LogoutFlag)
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
			if (recvMsg[1] == '0')td.lobby->PrintWaitionRoomList("0");
			else td.lobby->PrintWaitionRoomList(recvMsg + 1);
		}
		else if (recvMsg[0] == '/')
		{//채팅메시지 전용

		}
		else if (recvMsg[0] == '@')	//리퀘스트(req) 방만들기, 종료(로그아웃)요청완료 등 메시지 받는곳
		{							//방생성 실패 성공(@R0, @R1), 종료요청완료(@E1), 로그아웃
			if (recvMsg[1] == 'R')
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(hEventForRequest);
				}
			}
			else if (recvMsg[1] == 'L')//로그아웃요청완료(@L1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(hEventForRequest);
					LogoutFlag = true;
				}
			}
			else if (recvMsg[1] == 'G')//종료요청완료(@E1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(hEventForRequest);
					ExitFlag = true;
				}
			}
		}
		else { cout << "옳지않은 메세지: " << recvMsg << endl; Sleep(100); }
	}
	cout << "exitRecvMsgThread" << endl;
	return 0;
}
