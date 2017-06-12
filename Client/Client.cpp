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
	rThre.setUserInfo(&user);//ThreadData.user�� ��
	rThre.tData.lobby = &Lobby;
	rThre.tData.wRoom = &wRoom;
	rThre.tData.gPlay = &gPlay;

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
			rThre.ClientMode = 1;//Lobby
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
					while (/*user.IsCurrentClientMode() ||*/ (user.getClientMode() >= GameState::WAIT_ROOM))
					{
						wRoom.WatingRoomMain();
						if (user.getClientMode() == 3)// user.IsCurrentClientMode(GameState::GamePlay)
						{
							gPlay.mazeGameMain();
							rThre.ClientMode = 2; user.setClientMode(2);
						}
						else {}
						//���ӳ������ϱ� ClientMode 2�� ����
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
						//���ӳ������ϱ� ClientMode 2�� ����
					}
					break;
				case LOGOUT_CODE:
					rThre.LogoutFlag = true;
					rThre.threadOn = false;
					rThre.ClientMode = 0;//loginmain

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
	return 0;
}

