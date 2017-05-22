// Client.cpp : Defines the entry point for the console application.
//

#include"LoginMain.h"
#include"Lobby.h"

#define CREATE_ROOM 0
#define JOIN_ROOM 1
#define EXIT_CODE 2
#define LOGOUT_CODE 3
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
				�游���:? 3, ��������: 2, �α׾ƿ�: 1, ����: 0
				���ӳ����� �κ��(�ӽ�)
				*/
				switch (Lobby.LobbyMain())
				{
				case CREATE_ROOM:

					break;
				case JOIN_ROOM:

					break;
				case EXIT_CODE:
					ExitFlag = true;
					break;
				case LOGOUT_CODE:
					LogoutFlag = true;
					break;

				}
			}
		}
	}
	return 0;
}

