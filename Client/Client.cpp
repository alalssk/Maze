// Client.cpp : Defines the entry point for the console application.
//

#include"LoginMain.h"
#include"Lobby.h"

#define CREATE_ROOM 0
#define JOIN_ROOM 3
#define LOGOUT_CODE 1
#define EXIT_CODE 2

int main()
{
	LoginMain lmain;
	Lobby Lobby;
	bool ExitFlag = false, LogoutFlag = false;


	while (!ExitFlag)
	{

		if (lmain.LoginMainStart() == EXIT_CODE)//0 ��������, 1 ���Ӽ���
		{
			break;
		}
		else
		{
			LogoutFlag = false;
			while (!ExitFlag && !LogoutFlag)
			{
				/*
				LobbyMain
				�游���:0, ��������: 1, �α׾ƿ�: 2, ����: 3
				���ӳ����� �κ��(�ӽ�)
				*/
				switch (Lobby.LobbyMain(lmain.toServer.getSocket()))
				{
				case CREATE_ROOM:

					break;
				case JOIN_ROOM:

					break;
				case LOGOUT_CODE:
					LogoutFlag = true;
					//��Ĺ ���� �ʿ�
					break;
				case EXIT_CODE:
					ExitFlag = true;
					break;

				}
			}
			//
		}
	}
	return 0;
}

