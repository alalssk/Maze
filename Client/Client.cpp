// Client.cpp : Defines the entry point for the console application.
//

#include"LoginMain.h"
#include"Lobby.h"

#define CONNECTION_CODE 1
#define CREATE_ROOM 0
#define JOIN_ROOM 3
#define LOGOUT_CODE 4
#define EXIT_CODE 2

int main()
{
	LoginMain lmain;
	Lobby Lobby;
	
	int code;

	while (!Lobby.ExitFlag)
	{
		code = lmain.LoginMainStart();
		if (code == EXIT_CODE)//0 ��������, 1 ���Ӽ���
		{
			return 0;
			
		}
		else if (code == CONNECTION_CODE)
		{
			Lobby.LogoutFlag = false;
			while (!Lobby.ExitFlag && !Lobby.LogoutFlag)
			{
				/*
				LobbyMain
				�游���:0, ��������: 1, �α׾ƿ�: 4, ����: 3
				���ӳ����� �κ��(�ӽ�)
				*/
				switch (Lobby.LobbyMain(lmain.toServer.getSocket()))
				{
				case CREATE_ROOM:

					break;
				case JOIN_ROOM:

					break;
				case LOGOUT_CODE:

					//��Ĺ ���� �ʿ�
					break;
				case EXIT_CODE:

					break;

				}
			}
			lmain.toServer.CleanupSock();
			//
		}
	}
	return 0;
}

