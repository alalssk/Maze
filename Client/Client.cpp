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
	bool ExitFlag = false, LogoutFlag = false;
	int code;

	while (!ExitFlag)
	{
		code = lmain.LoginMainStart();
		if (code == EXIT_CODE)//0 접속종료, 1 접속성공
		{
			return 0;
			
		}
		else if (code == CONNECTION_CODE)
		{
			LogoutFlag = false;
			while (!ExitFlag && !LogoutFlag)
			{
				/*
				LobbyMain
				방만들기:0, 게임참가: 1, 로그아웃: 4, 종료: 3
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

