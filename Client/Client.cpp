// Client.cpp : Defines the entry point for the console application.
//

#include"LoginMain.h"
#include"Lobby.h"
#include"RecvThreadClass.h"
#include"WaitingRoom.h"

#define CONNECTION_CODE 1
#define CREATE_ROOM 0
#define JOIN_ROOM 3
#define LOGOUT_CODE 4
#define EXIT_CODE 2

int main()
{
	LoginMain lmain;
	Lobby Lobby;
	RecvThreadClass rThre;
	WaitingRoom wRoom;
	UserInfo user;
	int code;
	lmain.setUserInfo(&user);
	Lobby.setUserInfo(&user);
	wRoom.setUserInfo(&user);
	rThre.setUserInfo(&user);//ThreadData.user에 들어감
	rThre.tData.lobby = &Lobby;
	rThre.tData.wRoom = &wRoom;
	//wRoom.WatingRoomMain();

	while (!rThre.ExitFlag)
	{
		lmain.ConnectServer();
		rThre.tData.sock = lmain.toServer.getSocket();
//		rThre.tData.lobby = &Lobby;
		code = lmain.LoginMainStart();
		if (code == EXIT_CODE)//0 접속종료, 1 접속성공
		{
			return 0;
			
		}
		else if (code == CONNECTION_CODE)
		{
			rThre.LogoutFlag = false;
			rThre.StartThread();
			while (!rThre.ExitFlag && !rThre.LogoutFlag)
			{
				/*
				LobbyMain
				방만들기:0, 게임참가: 1, 로그아웃: 4, 종료: 3
				게임끝나면 로비로(임시)
				*/
				
				switch (Lobby.LobbyMain())
				{
				case CREATE_ROOM:

					//rThre.tData.wRoom.WatingRoomMain();
					wRoom.WatingRoomMain();
					break;
				case JOIN_ROOM:

					break;
				case LOGOUT_CODE:
					rThre.LogoutFlag = true;
					rThre.threadOn = false;
					//소캣 해제 필요
					break;
				case EXIT_CODE:
					rThre.ExitFlag = true;
					rThre.threadOn = false;
					break;

				}
			}
			
//			lmain.toServer.CleanupSock();
			lmain.~LoginMain();
			Lobby.~Lobby();
			//
		}
	}
	return 0;
}

