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
	string id, pass;
	char tmp_id_pass[20 + 20] = "";
	strcpy(tmp_id_pass, id_pass);
	char *tmp;
	tmp = strtok(tmp_id_pass, "_");
	id = tmp;
	tmp = strtok(NULL, "_");
	pass = tmp;

	char query[1024] = "";
	sprintf(query, "select user_id, user_pass from user_tbl where user_id = '%s';",id.c_str());
	cout << query << endl;
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
	string instrument = "INSERT INTO user_tbl (user_id, user_pass, user_WinCount, user_PlayCount) VALUES ('" + id + "', '" + pass +  "', 0, 0 )";
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