#pragma once
#include<iostream>
/*User Infomation*/
#define USER_ID_SIZE 13
#define USER_PASS_SIZE 20
/*User Infomation*/

class UserInfo
{
	char id[USER_ID_SIZE];
	char password[USER_PASS_SIZE];
	char userID_Password_for_send[USER_ID_SIZE + USER_PASS_SIZE+1];

public:
	UserInfo();
	~UserInfo();
	bool setID(char*);
	bool setPassword(char*);
	char* conv_ID_Password();
	void initUserInfo();


};