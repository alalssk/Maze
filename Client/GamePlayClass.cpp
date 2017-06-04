#include "GamePlayClass.h"


GamePlayClass::GamePlayClass() :key(0), x(2), y(2)
{
}


GamePlayClass::~GamePlayClass()
{
}

void GamePlayClass::setUserInfo(UserInfo *input_user)
{
	user = input_user;
}
int GamePlayClass::mazeGameMain()
{
	sock = user->getSocket();
	ClearXY();
	info.ReadMap();
	bool startFlag = false;
	char SendMsg[1024] = "";
	//여기왔을때 상태변경요청send "$S[방번호]_[ID] ID말고 번호로 하면 어떨까? 
	//서버에서 상태 바꾸고 $S1_[ID]전송(서버에서 해당하는 방 번호의 클라에만 전송해주는거니까 방번호를 적어줄 필요없음) 클라에서는 해당 ID상태 바꿔줌
	//0.5초마다 세명의 상태를 비교하여 보두 true가 되면 3초후 게임시작 
	//나는 게임시작 준비가 되었다 >> $S[방번호]_[ID] 을 서버로 보냄
	sprintf(SendMsg, "$S%d_%s", user->wData.RoomNum, user->getID());//$S[방번호]_[ID]
	send(sock, SendMsg, strlen(SendMsg) + 1, 0);
	while (!startFlag)
	{
		ClearXY();

		startFlag = true;
		for (int i = 0; i < user->wData.ConnectUserNum; i++)
		{
			startFlag = startFlag & (user->wData.UserState[i]);
		}
		cout << "StartFlag : ";
		if (startFlag)cout << "TRUE" << endl; else cout << "FALSE" << endl;
		cout << "방 번호 [" << user->wData.RoomNum << ']' << endl;
		cout << "방 이름: " << user->wData.RoomName << endl;
		cout << "방에 접속중인 유저 수: " << user->wData.ConnectUserNum << endl;
		for (int i = 0; i < user->wData.ConnectUserNum; i++)
		{
			cout << i << ". ID: " << user->wData.UserName[i] << " [승수-" << user->wData.winCount[i] << "] 상태(";
			if (user->wData.UserState[i])cout << "TRUE)" << endl; else cout << "FALSE)" << endl;;
		}
		Sleep(1000);
	}

	ClearXY(); info.PrintThree(); Sleep(1500);
	ClearXY(); info.PrintTwo(); Sleep(1500);
	ClearXY(); info.PrintOne(); Sleep(1000);

	gotoxy(1, 1);
	info.grideMap();
	while (1)
	{

		gotoxy(x, y), cout << '*';//플레이어 표시(임시)
		gotoxy(1, 1);
		//move_arrow(getKeyDirectionCheck());//자동탐색 만들떄 getKey...함수 뺴고 자동으로 키입력하는 함수 만들면 될듯
		SendInputKey(getKeyDirectionCheck());
		//여기서 나의 좌표 send
		if (finalPoint())//골인지점
		{
			ClearXY();
			//	Ending();
			//userlog.PrintUserlog();
			key = 0, x = 2, y = 2;//init
			return 0;
		}
		else
		{
			gotoxy(x, y);
			cout << '*';
		}
	}

}

void GamePlayClass::initPrint()
{
	gotoxy(64, 20);
	cout << "           ";
}
void GamePlayClass::movePrint()
{
	gotoxy(64, 20);
	cout << " 벽입니다 ";
}
void GamePlayClass::Ending()
{
	string cong = "축하합니다!!!";
	string end_comment = "게임을 종료 합니다!!!";
	gotoxy(X_MAX / 2 - cong.size() / 2 - 1, Y_MAX / 2), cout << cong;
	for (int i = 1; i < Y_MAX / 2 + 1; i++)
	{
		EndingSubLoop(i, '#');
		Sleep(100);
	}

	for (int i = Y_MAX / 2 + 1; i >= 1; i--)
	{
		if (i > 3)
		{
			gotoxy(X_MAX / 2 - end_comment.size() / 2 - 1, Y_MAX / 2), cout << end_comment;
		}
		EndingSubLoop(i, ' ');
		Sleep(100);
	}

}
void GamePlayClass::EndingSubLoop(int a, char c)//ENDING
{
	int i, j;

	for (i = 0 + a; i < X_MAX - a; i++)
	{
		gotoxy(i, a), cout << c;
		//	Sleep(SPEED);
	}

	for (j = 0 + a; j < Y_MAX - a; j++)
	{
		gotoxy(i - 1, j), cout << c;
		//		Sleep(SPEED);
	}
	for (; i > 0 + a; i--)
	{
		gotoxy(i - 1, j), cout << c;
		//		Sleep(SPEED);
	}
	for (; j > 0 + a; j--)
	{
		gotoxy(i, j), cout << c;
		//		Sleep(SPEED);
	}
	gotoxy(45, 30);
}
bool GamePlayClass::finalPoint()
{
	if (info.gameMap[y - 1][x - 1] == 'F') return TRUE;
	else return FALSE;
}
void GamePlayClass::move_arrow(int key)//x,y 플레이어 좌표값 (임시)
{
	initPrint();

	switch (key)
	{
	case UP://위로
		if (info.gameMap[y - 2][x - 1] == '#') {
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			y -= 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case LEFT: //왼쪽
		if (info.gameMap[y - 1][x - 2] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			x -= 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case RIGHT: //오른쪽
		if (info.gameMap[y - 1][x] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			x += 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	case DOWN: //아래
		if (info.gameMap[y][x - 1] == '#'){
			movePrint();
		}
		else {
			gotoxy(x, y); cout << ' ';
			y += 1;
			//userlog.InputLog(x, y, 0);
		}
		break;
	default:
		gotoxy(64, 1);
		cout << "방향키만입력 (종료는q)" << endl;
		return;
	}
}

bool GamePlayClass::SendInputKey(int key)
{// P방번호_유저키_방향키
	initPrint();//임시
	char SendMsg[13] = "";
	switch (key)
	{
	case UP://위로
		if (info.gameMap[y - 2][x - 1] == '#') {
			//벽
			return false;
		}
		else {
			gotoxy(x, y); cout << ' ';
			y -= 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), key);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	case LEFT: //왼쪽
		if (info.gameMap[y - 1][x - 2] == '#'){
			//벽
			return false;
		}
		else {
			gotoxy(x, y); cout << ' ';
			x -= 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), key);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	case RIGHT: //오른쪽
		if (info.gameMap[y - 1][x] == '#'){
			//벽
			return false;
		}
		else {
			gotoxy(x, y); cout << ' ';
			x += 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), key);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	case DOWN: //아래
		if (info.gameMap[y][x - 1] == '#'){
			//벽
			return false;
		}
		else {
			gotoxy(x, y); cout << ' ';
			y += 1;
			sprintf(SendMsg, "P%d_%d_%d", user->wData.RoomNum, user->GetRoomUserKey(), key);
			send(sock, SendMsg, strlen(SendMsg) + 1, 0);
		}
		break;
	default:
		gotoxy(64, 1);
		cout << "방향키만입력 (종료는q)" << endl;
		return false;
	}

	return true;
}