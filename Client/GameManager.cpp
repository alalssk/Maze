#include "GameManager.h"


GameManager::GameManager()
{
}


GameManager::~GameManager()
{
}
void GameManager::initGame()
{
	user.initUserInfoData();
	lmain.setUserInfo(&user);
	Lobby.setUserInfo(&user);
	wRoom.setUserInfo(&user);
	gPlay.setUserInfo(&user);
	rThre.setUserInfo(&user);//ThreadData.user에 들어감
	rThre.tData.lobby = &Lobby;
	rThre.tData.wRoom = &wRoom;
	rThre.tData.gPlay = &gPlay;

}

bool GameManager::RunGame()
{

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
			user.setClientMode(user.GameState::LOBBY);
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
					while (	user.IsCurrentClientMode(user.GameState::WAIT_ROOM) || 
							user.IsCurrentClientMode(user.GameState::GAMEPLAY))
					{
						wRoom.WatingRoomMain();
						if (user.IsCurrentClientMode(user.GameState::GAMEPLAY))
						{
							gPlay.mazeGameMain();
							user.setClientMode(user.GameState::WAIT_ROOM);
						}
						else {}
					}
					break;
				case JOIN_ROOM:
					while (	user.IsCurrentClientMode(user.GameState::WAIT_ROOM) || 
							user.IsCurrentClientMode(user.GameState::GAMEPLAY))
					{
						wRoom.WatingRoomMain();
						if (user.IsCurrentClientMode(user.GameState::GAMEPLAY))
						{
							gPlay.mazeGameMain();
							user.setClientMode(user.GameState::WAIT_ROOM);
						}
						else {}
						//게임끝났으니까 ClientMode 2로 변경
					}
					break;
				case LOGOUT_CODE:
					rThre.LogoutFlag = true;
					rThre.threadOn = false;
					user.setClientMode(user.GameState::LOGIN);

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
}