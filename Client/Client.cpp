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

		if (lmain.LoginMainStart() == EXIT_CODE)//0 접속종료, 1 접속성공
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
				방만들기:0, 게임참가: 1, 로그아웃: 2, 종료: 3
				게임끝나면 로비로(임시)
				*/
				switch (Lobby.LobbyMain(lmain.toServer.getSocket()))
				{
				case CREATE_ROOM:

					break;
				case JOIN_ROOM:

					break;
				case LOGOUT_CODE:
					LogoutFlag = true;
					//소캣 해제 필요
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

