#include "WaitingRoom.h"


WaitingRoom::WaitingRoom()
{
}


WaitingRoom::~WaitingRoom()
{
}
void WaitingRoom::setUserInfo(UserInfo *input_user)
{
	user = input_user;
}
int WaitingRoom::WatingRoomMain()
{
	ClearXY();
	sock = user->getSocket();
	//user.setUserID("alalssk");/*###임시 채팅로그 만드는중*/
	gotoxy(5, 3);	cout << " No. " << 1234567;//gotoxy(5+5,3); cout<<RoomNum;
	gotoxy(5 + 15, 3); cout << "Name: "<< " 방제목방제목방제목";
	GrideBox(5, 4, 3, 18);//PrintUserListBox
	PrintUserList();
	GrideBox(5, 9, 20, 18);//PrintChatBox
	//PrintChat()
	PrintButton();
	char inputstr[MAXCHAR];
	int inputstrSz = 0;
	memset(inputstr, 0, sizeof(inputstr));
	while (1)
	{
		key = getKeyDirectionCheck();
		WRinfo.SetWaitingRoomFlag(key);//왼쪽 오른쪽 구분(메뉴선택, 채팅)
		if (WRinfo.GetWaitingRoomFlag() == false)
		{//메뉴선택부분
			if (key == ENTER)
			{	//종료부분 방만 나가는걸로 게임종료는 로비랑 로그인 화면에서만
				//@E_[방번호]_[ID]
				if (WRinfo.GetWaitingRoomTxtNum() == 2)break;
				
			}
			else
			{
				WRinfo.SetWaitingRoomTxtNum(key);


				PrintButton();
			}
			
		}
		else
		{//채팅부분 구현

			if (key == ENTER)
			{//서버연결후 고칠부분
				//
				//ChattingSendToServer(string chat) >> 이 함수 안에 InputChatLog("alalssk", inputstr); 이함수를 넣음
				//여기선 채팅을 버서로 보내기만하고 inputstr이랑 sz초기화해준다
				//채팅 출력과  채팅창 초기화는 Recv스레드에서 할것임

				InputChatLog("alalssk", inputstr);
				memset(inputstr,0,sizeof(inputstr)); inputstrSz = 0;
				initChatListBox();//채팅창 초기화
				PrintChatLogList();//출력
				gotoxy(7, 28);
			}
			else
			{
				//initChatListBox();
				if (key != LEFT &&key != RIGHT && key != UP && key != DOWN &&key != ENTER)inputstr[inputstrSz++] = key;
				gotoxy(7, 28); 	cout << inputstr;
			}


			//채팅방 생성시 채팅출력용 스레드 만들어서 채팅list에 입력이있으면(타클라로부터(recv) 채팅출력


		}
	}
	return 0;
}

void WaitingRoom::PrintUserList()
{
	for (int i = 0; i < 3; i++)//MAX user 3
	{
		gotoxy(7, 5 + i);
		cout << "READY!!" << "  ||  " << "DIDIDIDIDIDID" << "  ||  " << 15 << endl;//state || ID || WinCount
	}
}
void WaitingRoom::PrintButton()
{
	AllClearPrintLobbyTxtBox();
	GrideBox(50, 4 * WRinfo.GetWaitingRoomTxtNum(), 1, 7);// 4 * ButtonNumber(1,2)
	gotoxy(56, 5); cout << "Start !";//or Ready !!
	gotoxy(56, 9); cout << "Exit !!";
}
void WaitingRoom::AllClearPrintLobbyTxtBox()
{
	for (int i = 0; i < 8; i++)
	{
		gotoxy(50, 4 + i);
		cout << "                    ";
	}
}
bool WaitingRoom::InputChatLog(string name, string chat)
{
	ChatLog.push_back(pair<string, string>(name, chat));
	return true;//예외처리 추가하셈
}
void WaitingRoom::PrintChatLogList()
{
	//7,28 부터 y값 --
	for (int i = 1; i <= 17; i++)
	{
		if (ChatLog.size() < i)break;
		gotoxy(7, 27 - i);
		cout << ChatLog[ChatLog.size() - i].first << ": " << ChatLog[ChatLog.size() - i].second;
	}

}
void WaitingRoom::initChatListBox()
{
	for (int i = 0; i < 18; i++)
	{
		gotoxy(7, 11 + i);
		cout << "                                    ";
	}
}