#include "Lobby.h"


Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}
const int Lobby::LobbyMain(SOCKET sock)
{
	setSock(sock);
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
			if (key == SPACE || key == ENTER) return 4;
		}
		else if (Linfo.GetLobbyTxtNum() == 2)// exit
		{
			if (key == SPACE || key == ENTER) return 2;
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
	char recvBuf[3] = "";
	send(sock, "@R", 2, 0);
	recv(sock, recvBuf, 3, 0);
	//실패(@R0) or 성공(@R1)
	gotoxy(5, 30); cout << "req_createRoom: "<<recvBuf;
	if (recvBuf[2] == '1')return true;
	else return false;
	
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