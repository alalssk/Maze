#include "RecvThreadClass.h"

int RecvThreadClass::ClientMode = 0;
bool RecvThreadClass::threadOn = false;//임시
bool RecvThreadClass::ExitFlag = false;
bool RecvThreadClass::LogoutFlag = false;
RecvThreadClass::RecvThreadClass()
{
}


RecvThreadClass::~RecvThreadClass()
{
}
void RecvThreadClass::StartThread()
{

	if (!threadOn)
	{
		hRcvThread = (HANDLE)_beginthreadex(NULL, 0, &RecvMsg, (void*)&tData, 0, NULL);
		threadOn = true;

	}

}
void RecvThreadClass::setUserInfo(UserInfo* input_user)
{
	tData.user = input_user;
}
unsigned WINAPI RecvThreadClass::RecvMsg(void * arg)   // read thread main
{
	ThreadData tData = *((ThreadData*)arg);
	char recvMsg[BUF_SIZE] = "";
	char *tmp_tok;
	int itmp_RoomNum;
	int strLen;
	//ClientMode >= 1(lobby상태) 일때 넘어가도록 이벤트 처리 WaitSingleObject(hEvent,INFINITE);
	//그럼 로그아웃할때 스레드도 종료시켜줘야겠지

	while (!ExitFlag && !LogoutFlag)
	{
		strLen = recv(tData.sock, recvMsg, BUF_SIZE - 1, 0);
		//이 부분에 send스레드에서 종료플레그가 켜지면 이벤트 처리를....
		//방법2. 소캣을 넌블로킹으로 만들기 (ioctlsocket)
		if (strLen == -1)
			return -1;

		if (recvMsg[0] == '!')
		{
			//방정보 받아옴==> !"방개수"_"No.[방번호]>> [방이름]"_...
			//!0 이면 방이없다는 말임
			//일단 서버에서 ! 요고만 보내고 방정보 함수가 제대로 동작하는지 체크하자
			if (recvMsg[1] == '0')tData.lobby->GetWaitionRoomList("0");
			else tData.lobby->GetWaitionRoomList(recvMsg + 1);
			memset(recvMsg, 0, sizeof(recvMsg));
		}
		else if (recvMsg[0] == '/')
		{//채팅메시지 전용

		}
		else if (recvMsg[0] == '@')	//리퀘스트(req) 방만들기, 종료(로그아웃)요청완료 등 메시지 받는곳
		{							//방생성 실패 성공(@R0, @R1), 종료요청완료(@E1), 로그아웃
			if (recvMsg[1] == 'R')
			{
				if (recvMsg[2] == '1')
				{
					////////////////
					//만들었으니까 방정보를 받아와야겠지
					//@R1_[방번호]_[방제목]
					//받는거 확인
					tData.user->setWaitingRoomData(recvMsg+4); //[방번호]_[방제목]
					SetEvent(tData.lobby->hEventForRequest);
				}
			}
			else if (recvMsg[1] == 'r')//방요청(시작시)
			{
				if (recvMsg[2] == '1')
				{

					SetEvent(tData.lobby->hEventForRequest);
				}
			}
			else if (recvMsg[1] == 'L')//로그아웃요청완료(@L1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(tData.lobby->hEventForRequest);
					LogoutFlag = true;
				}
			}
			else if (recvMsg[1] == 'G')//종료요청완료(@G1)
			{
				if (recvMsg[2] == '1')
				{
					SetEvent(tData.lobby->hEventForRequest);
					ExitFlag = true;
				}
			}
		}
		else {}
	}
	cout << "exitRecvMsgThread" << endl;
	return 0;
}