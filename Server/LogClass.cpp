#include "LogClass.h"


LogClass::LogClass()
{

}


LogClass::~LogClass()
{
	wf.close();
}
void LogClass::OpenFile(char * fileName)
{
	wf.open(fileName, ios_base::out);
	if (wf.is_open())
	{
		cout << '[' << fileName << "]���Ͽ���" << endl;
	}
	else
	{
		cout << '[' << fileName << "]���Ͽ��� ����" << endl;
	}
}
void LogClass::In_ChatLog_txt(char* msg)
{

	wf << msg << endl;
}