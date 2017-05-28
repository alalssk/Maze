#pragma once
/*방 정보에 대한 것만*/
#include"grideXY.h"
#include<list>


class WaitingRoomInfo
{
	typedef struct
	{
		int RoomNumber;
		char RoomName[40];
		char OwnerID[20];
		//userInfo class 가 만들어 지면 여기에d
		//userInfo users[3];
		//int userCount; (max값 3)
		//이는 멀티기능을 구현할 때 필요함.
	}RoomInfo;
	list <RoomInfo> Roomlist;//이게 클라에서 필요한가...? 로비에서 방 리스트 출력해주려면 어쩔수없나...?
	list<string> chatMemory; // 출력용 채팅로그 리스트
	/*WaitingRoom 기능 관리 함수, 변수*/

	bool WaitingRoomFlag;
	int WaitingRoomTxtNum;
	int gitgit;//github확인용
public:
	const bool GetWaitingRoomFlag();
	void SetWaitingRoomFlag(const int key);// chat or choose(start, exit)
	const int GetWaitingRoomTxtNum();
	void SetWaitingRoomTxtNum(const int key); //start or exit
	//


public:
	WaitingRoomInfo();
	~WaitingRoomInfo();

	bool RoomsInfoFromServer();
	list<RoomInfo> getRoomInfo();


};

