#include "stdafx.h"
#include "ServerDB.h"


ServerDB::ServerDB()
{
}


ServerDB::~ServerDB()
{
}

void ServerDB::StartDB()
{
	printf("Mysql Client Version: %s\n", mysql_get_client_info());

}