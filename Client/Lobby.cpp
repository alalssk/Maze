#include "Lobby.h"


Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}
const int Lobby::LobbyMain()
{
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
			//Linfo.SetLobbyListNumber(key);
			//initRoomListCheck();
			//PrintLobbyListCheck(Linfo.GetLobbyListNumber());

			//AllClearPrintLobbyTxtBox();
			//PrintLobbyTxt();
		}



		if (Linfo.GetLobbyTxtNum() == 0) //�游���
		{
			if (key == SPACE || key == ENTER)
			{
				return 0; 
			}
		} 
		//else if (Linfo.GetLobbyTxtNum() == 1)
		//{
		//	if (key == SPACE || key == ENTER)
		//	{
		//		//ClearXY();

		//		return 1; //��������(join)
		//	}
		//} �޴� ������ �� �������� ���� �����ϰ� �ҰŴϱ�!
		else if (Linfo.GetLobbyTxtNum() == 2) //logoout
		{
			if (key == SPACE || key == ENTER) return 2;
		}
		else if (Linfo.GetLobbyTxtNum() == 3)// exit
		{
			if (key == SPACE || key == ENTER) return 3;
		}
	}

}
void Lobby::PrintLobbyListBox()
{
	int i = 0;
	gotoxy(16, 3);
	cout << " �� �� �� �� ��" << endl;
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
			cout << "�� �� �� ��"; break;
		case 1:
			cout << "�� �� �� ��"; break;
		case 2:
			cout << "�� �� �� ��"; break;
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