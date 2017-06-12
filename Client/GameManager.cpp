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
	rThre.setUserInfo(&user);//ThreadData.user�� ��
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
		if (code == EXIT_CODE)//0 ��������, 1 ���Ӽ���
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
				�游���:0, ��������: 1, �α׾ƿ�: 4, ����: 3
				���ӳ����� �κ��(�ӽ�)
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
						//���ӳ������ϱ� ClientMode 2�� ����
					}
					break;
				case LOGOUT_CODE:
					rThre.LogoutFlag = true;
					rThre.threadOn = false;
					user.setClientMode(user.GameState::LOGIN);

					//��Ĺ ���� �ʿ�
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