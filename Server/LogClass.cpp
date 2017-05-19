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
		cout << '[' << fileName << "]파일열림" << endl;
	}
	else
	{
		cout << '[' << fileName << "]파일열기 실패" << endl;
	}
}
void LogClass::In_ChatLog_txt(char* msg)
{

	wf << msg << endl;
}