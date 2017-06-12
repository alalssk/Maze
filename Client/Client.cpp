// Client.cpp : Defines the entry point for the console application.
//

#include"LoginMain.h"
#include"Lobby.h"
#include"RecvThreadClass.h"
#include"WaitingRoom.h"
#include"GamePlayClass.h"
#include"GamePlayInfo.h"

#define CONNECTION_CODE 1
#define CREATE_ROOM 0
#define JOIN_ROOM 3
#define LOGOUT_CODE 4
#define EXIT_CODE 2

enum GameState
{
	LOGIN = 0,
	LOBBY,
	WAIT_ROOM,
	GAMEPLAY,
	GAMERESULT,
};

int main()
{
	LoginMain lmain;
	Lobby Lobby;
	RecvThreadClass rThre;
	WaitingRoom wRoom;
	GamePlayClass gPlay;
	UserInfo user;
	int code;
	//user.initUserInfo();
	user.initUserInfoData();
	lmain.setUserInfo(&user);
	Lobby.setUserInfo(&user);
	wRoom.setUserInfo(&user);
	gPlay.setUserInfo(&user);
	rThre.setUserInfo(&user);//ThreadData.user에 들어감
	rThre.tData.lobby = &Lobby;
	rThre.tData.wRoom = &wRoom;
	rThre.tData.gPlay = &gPlay;

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
			rThre.ClientMode = 1;//Lobby
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
					//Get Set Is From Add Append Remove Initialize Finalize Begin End Empty
					//rThre.tData.wRoom.WatingRoomMain();
					while (/*user.IsCurrentClientMode() ||*/ (user.getClientMode() >= GameState::WAIT_ROOM))
					{
						wRoom.WatingRoomMain();
						if (user.getClientMode() == 3)// user.IsCurrentClientMode(GameState::GamePlay)
						{
							gPlay.mazeGameMain();
							rThre.ClientMode = 2; user.setClientMode(2);
						}
						else {}
						//게임끝났으니까 ClientMode 2로 변경
					}
					break;
				case JOIN_ROOM:
					while ((user.getClientMode() >= 2))
					{
						wRoom.WatingRoomMain();
						if (user.getClientMode() == 3)
						{
							gPlay.mazeGameMain();
							rThre.ClientMode = 2; user.setClientMode(2);
						}
						else {}
						//게임끝났으니까 ClientMode 2로 변경
					}
					break;
				case LOGOUT_CODE:
					rThre.LogoutFlag = true;
					rThre.threadOn = false;
					rThre.ClientMode = 0;//loginmain

					//소캣 해제 필요
					break;
				case EXIT_CODE:
					rThre.ExitFlag = true;
					rThre.threadOn = false;
					break;
				}
			}//end while

			//lmain.toServer.CleanupSock();
			lmain.~LoginMain();
			Lobby.~Lobby();
			//
		}
	}
	return 0;
}

