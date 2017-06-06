#include "ServerDB.h"


ServerDB::ServerDB()
{
}


ServerDB::~ServerDB()
{
	mysql_close(connection);
}

bool ServerDB::StartDB()
{
	printf("Mysql Client Version: %s\n", mysql_get_client_info());

	mysql_init(&conn);
	connection = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char *)NULL, 0);
	if (connection == NULL)
	{
		fprintf(stderr, "Mysql connection error : %s", mysql_error(&conn));
		return false;
	}

	return true;

}

bool ServerDB::Check_Password(char* id_pass)//"[ID]_[PASSWORD]"
{
	MYSQL_RES * sql_result;
	MYSQL_ROW sql_row;
	int query_stat;
	string id, pass;
	char tmp_id_pass[20 + 20] = "";
	strcpy(tmp_id_pass, id_pass);
	char *tmp;
	tmp = strtok(tmp_id_pass, "_");
	id = tmp;
	tmp = strtok(NULL, "_");
	pass = tmp;

	char query[1024] = "";
	sprintf(query, "select user_id, user_pass from user_tbl where user_id = '%s';", id.c_str());
	query_stat = mysql_query(connection, query);
	if (query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}
	else{
		sql_result = mysql_store_result(connection);
		//cout << mysql_num_rows(sql_result) << endl;
		if (mysql_num_rows(sql_result) == 0)
		{//ID가 DB에 없는 경우 insert
			if (Insert_User(id, pass))
			{
				return true;
			}
			else false;
		}
		else
		{//password 검사
			sql_row = mysql_fetch_row(sql_result);
			if (strcmp(sql_row[1], pass.c_str()) == 0)
			{
				//cout << "비번 같음" << endl;
				OneIncreass_visit_count(id);
				return true;
			}
			else {
				return false;
			}
		}

	}
	return true;

}
bool ServerDB::Insert_User(string id, string pass)
{
	int query_stat;
	string instrument = "INSERT INTO user_tbl (user_id, user_pass) VALUES ('" + id + "', '" + pass + "')";
	//string instrument = "INSERT INTO user (id, password, connect_IP) VALUES ( '" + id + "', '" + password + "', '" + ip + "')";
	//insert 예문: insert into user_tbl(user_id, user_pass, user_WinCount, user_PlayCount) values('[ID입력]', '[PASS입력]', 0, 0); 
	//varchar 형은 입력할때 ''나 "" 해줘야함
	query_stat = mysql_query(connection, instrument.c_str());
	if (query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		cout << endl;
		cout << instrument << endl;
		return false;
	}
	cout << "OK insert" << endl;
	return true;
}
bool ServerDB::OneIncreass_visit_count(string id)
{
	int query_stat;
	char query[1024] = "";
	sprintf(query, "update user_tbl set visit_count = visit_count+1 where user_id = '%s';", id.c_str());
	query_stat = mysql_query(connection, query);
	if (query_stat != 0)
	{
		fprintf(stderr, "[OneIncreass_visit_count]Mysql query error : %s\n", mysql_error(&conn));
		return false;
	}
	return true;
}
bool ServerDB::GetUserWinCount(string id, int &win, int &play)
{
	MYSQL_RES * sql_result;
	MYSQL_ROW sql_row;
	int query_stat;

	char query[1024] = "";
	sprintf(query, "select win_count, play_count from user_tbl where user_id = '%s';", id.c_str());
	query_stat = mysql_query(connection, query);
	if (query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return 1;
	}
	else{
		sql_result = mysql_store_result(connection);

		sql_row = mysql_fetch_row(sql_result);
		cout << sql_row[0] << ':' << sql_row[1] << endl;
		win = atoi(sql_row[0]);
		play = atoi(sql_row[1]);

	}
}


//0606
int ServerDB::GetTotalCreateRoomCount()
{
	MYSQL_RES * sql_result;
	MYSQL_ROW sql_row;
	int query_stat;

	string query = "select max(room_id) from room_tbl;";
	query_stat = mysql_query(connection, query.c_str());
	if (query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		return -1;
	}
	else{
		sql_result = mysql_store_result(connection);

		sql_row = mysql_fetch_row(sql_result);
		return atoi(sql_row[0]);

	}
}
bool ServerDB::CreateWaitingRoom(string RoomName, int RoomNum)//방이름, 방번호
{//insert
	int query_stat;
	char cRoomNum[10] = "";
	string sRoomNum;
	string sCreateTime;
	char cTime[25] = "";
	_itoa(RoomNum, cRoomNum, 10);
	sRoomNum = cRoomNum;
	/*이 부분 함수화 string func()*/
	GetSystemTime(&sysTime);
	sprintf(cTime, "%d-%d-%d %d:%d:%d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	sCreateTime = cTime;
	
	string instrument = "insert into room_tbl(room_name, room_num, create_time) VALUES ('" + RoomName + "', " + sRoomNum + ",'" + sCreateTime + "')";
	query_stat = mysql_query(connection, instrument.c_str());
	if (query_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
		cout << endl;
		cout << instrument << endl;
		cout << "[DB] Create Room is failed" << endl;
		return false;
	}
	cout << "[DB] Create Room is ok" << endl;
	return true;
}
bool ServerDB::DeleteWaitingRoom(string RoomName, int RoomNum)
// >> 이 룸넘버로 검색하면 중복되는 경우가 생김 (현재 room_id가 PK고 room_num은 서버를 다시시작하기까지만 고유한 번호임)
{//update 여기서 채팅 로그도 같이 포함하면 될듯
	//int query_stat;
	//char query[1024] = "";
	//sprintf(query, "update room_tbl set visit_count = visit_count+1 where user_id = '%s';", id.c_str());
	//query_stat = mysql_query(connection, query);
	//if (query_stat != 0)
	//{
	//	fprintf(stderr, "[OneIncreass_visit_count]Mysql query error : %s\n", mysql_error(&conn));
	//	return false;
	//}
	return true;
}

bool ServerDB::StartPlayGame(int RoomNum) //방번호
{//insert
	//int query_stat;
	//string instrument = "INSERT INTO user_tbl (user_id, user_pass) VALUES ('" + id + "', '" + pass + "')";
	//query_stat = mysql_query(connection, instrument.c_str());
	//if (query_stat != 0)
	//{
	//	fprintf(stderr, "Mysql query error : %s", mysql_error(&conn));
	//	cout << endl;
	//	cout << instrument << endl;
	//	return false;
	//}
	//cout << "OK insert" << endl;
	return true;
}
bool ServerDB::EndPlayGame(int RoomNum)
{//update
	return true;
}