#include "Lobby.h"

HANDLE Lobby::hEventForRequest;

Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}
const int Lobby::LobbyMain()
{
	sock = user->getSocket();
	hEventForRequest = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClearXY();
	PrintLobbyListBox();
	GrideBox(46, 6 + (Linfo.GetLobbyTxtNum() * 3), 1, 6);
	Sleep(50);
	req_GetWaitingRoom();
	Sleep(50);
	PrintWaitionRoomList();
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
			//PrintWaitionRoomList();

			PrintLobbyTxt();
		}
		else
		{

			//RoomInfoListMain();
			
			Linfo.SetLobbyListPointNumber(key);
			initRoomListCheck();
			PrintWaitionRoomList();
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
void Lobby::setUserInfo(UserInfo *user)
{
	this->user = user;
}
void Lobby::setSock(SOCKET sock)
{
	this->sock = sock;
}
bool Lobby::req_GetWaitingRoom()//방요청(시작시)
{
	send(sock, "@r", 2, 0);
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_CreateRoom()
{
	send(sock, "@R", 2, 0);
	//cout << "방생성요청보냄" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest,3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_LogoutClient()
{
	send(sock, "@L", 2, 0);
	//cout << "로그아웃요청보냄" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
bool Lobby::req_ExitClient()
{
	send(sock, "@G", 2, 0);
	//cout << "게임종료요청보냄" << endl;
	DWORD ret;
	ret = WaitForSingleObject(hEventForRequest, 3000);
	if (ret == WAIT_TIMEOUT) return false;
	else return true;
}
void Lobby::PrintWaitionRoomList()
{
	initPrintWaitingRoomList();
	gotoxy(0, 0); Sleep(100);
	for (int i = 0; i < Linfo.WaitingRoomListNum; i++)
	{
		gotoxy(7 + 3, 5 + i); cout << Linfo.WaitingRoomList[i];

	}
}
void Lobby::GetWaitionRoomList(char *buf)
{//[방개수]_[No.[방번호] 방이름]_[방이름]_[방이름]
	//WaitingRoomCount
	char *tmp;
	int iRoomCount;
	
	memset(Linfo.WaitingRoomList, 0, sizeof(Linfo.WaitingRoomList));//2차원배열(tmp[max][max]) 초기화하는 법 확실하게 테스트 하자
																	//memset(tmp,0,sizeof(tmp)) 해도 2차원 다 초기화 된다.
	if (buf[0] == '0')
	{//방이없음
		Linfo.WaitingRoomListNum = 0;
	}
	else
	{//방이 있는경우

		tmp = strtok(buf, "_");
		iRoomCount = atoi(tmp);
		Linfo.WaitingRoomListNum = iRoomCount;
		for (int i = 0; i < Linfo.WaitingRoomListNum; i++)
		{
			tmp = strtok(NULL, "_");
			strcpy(Linfo.WaitingRoomList[i], tmp);
			
		}
	}

	PrintWaitionRoomList();

}

void Lobby::initPrintWaitingRoomList()
{
	for (int i = 0; i < 14; i++)
	{
		gotoxy(7 + 3, 5 + i);
		cout << "                              ";
	}
}

void Lobby::PrintLobbyListCheck(int WaitingRoomListPointNumber)
{
	gotoxy(7, 5 + WaitingRoomListPointNumber);
	cout << "☞ ";
}


