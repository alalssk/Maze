#pragma once
#include<iostream>
#include<fstream>
using namespace std;


class LogClass
{
	ofstream wf;

public:

	LogClass();
	~LogClass();
	void In_ChatLog_txt(char* msg);
	void OpenFile(char* fileName);
};
