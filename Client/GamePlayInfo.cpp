#include "GamePlayInfo.h"


GamePlayInfo::GamePlayInfo()
{
	memset(One, 0, sizeof(One));
	memset(Two, 0, sizeof(Two));
	memset(Three, 0, sizeof(Three));
	strcpy(One[0], "                    ");
	strcpy(One[1], "                    ");
	strcpy(One[2], "            ■      ");
	strcpy(One[3], "          ■■      ");
	strcpy(One[4], "        ■■■      ");
	strcpy(One[5], "      ■■■■      ");
	strcpy(One[6], "          ■■      ");
	strcpy(One[7], "          ■■      ");
	strcpy(One[8], "          ■■      ");
	strcpy(One[9], "          ■■      ");
	strcpy(One[10], "          ■■      ");
	strcpy(One[11], "          ■■      ");
	strcpy(One[12], "          ■■      ");
	strcpy(One[13], "      ■■■■■    ");
	strcpy(One[14], "      ■■■■■    ");

	strcpy(Two[0], "                    ");
	strcpy(Two[1], "                    ");
	strcpy(Two[2], "        ■■        ");
	strcpy(Two[3], "      ■■■■      ");
	strcpy(Two[4], "    ■■    ■■    ");
	strcpy(Two[5], "    ■■    ■■    ");
	strcpy(Two[6], "    ■■    ■■    ");
	strcpy(Two[7], "            ■■    ");
	strcpy(Two[8], "            ■■    ");
	strcpy(Two[9], "          ■■      ");
	strcpy(Two[10],"        ■■        ");
	strcpy(Two[11],"      ■■          ");
	strcpy(Two[12],"    ■■            ");
	strcpy(Two[13],"    ■■■■■■    ");
	strcpy(Two[14],"    ■■■■■■    ");

	strcpy(Three[0], "                    ");
	strcpy(Three[1], "        ■■        ");
	strcpy(Three[2], "      ■■■■      ");
	strcpy(Three[3], "    ■■    ■■    ");
	strcpy(Three[4], "    ■■    ■■    ");
	strcpy(Three[5], "    ■■    ■■    ");
	strcpy(Three[6], "            ■■    ");
	strcpy(Three[7], "        ■■■      ");
	strcpy(Three[8], "        ■■■      ");
	strcpy(Three[9], "            ■■    ");
	strcpy(Three[10], "    ■■    ■■    ");
	strcpy(Three[11], "    ■■    ■■    ");
	strcpy(Three[12], "    ■■    ■■    ");
	strcpy(Three[13], "      ■■■■      ");
	strcpy(Three[14], "        ■■        ");
}

void GamePlayInfo::PrintOne()
{
	for (int i = 0; i < 15; i++)
	{
		cout << One[i] << endl;
		
	}
}
void GamePlayInfo::PrintTwo()
{
	for (int i = 0; i < 15; i++)
	{
		cout << Two[i] << endl;

	}
}
void GamePlayInfo::PrintThree()
{
	for (int i = 0; i < 15; i++)
	{
		cout << Three[i] << endl;

	}
}
GamePlayInfo::~GamePlayInfo()
{
	if (!gameMap == NULL)//이 조건문을 뺴면 처음 로그인 화면에서 게임 종료시 에러남
	{
		for (int i = 0; i < Y_SIZE; i++)
			delete[] gameMap[i];
		delete[] gameMap;
	}
}
void GamePlayInfo::ReadMap()
{
	gameMap = new char*[Y_SIZE + 1];
	for (int i = 0; i < Y_SIZE; i++)
	{
		gameMap[i] = new char[X_SIZE + 1];
		memset(gameMap[i], 0, sizeof(char)*X_SIZE + 1);
	}

	char buf[256];
	int len, i = 0;;
	FILE *readMapFilePointer = fopen("maze_map.txt", "rt");
	if ((readMapFilePointer = fopen("maze_map.txt", "r")) == NULL){
		perror("maze_map.txt: ");
		exit(1);
	}
	while (1){
		fgets(buf, X_SIZE + 1, readMapFilePointer);
		len = strlen(buf);
		if (len == 0) break;
		strcpy(gameMap[i], buf); i++;
		buf[0] = '\0';

	}

	fclose(readMapFilePointer);
}
void GamePlayInfo::grideMap()
{
	for (int i = 0; i < Y_SIZE; ++i){
		for (int j = 0; j < X_SIZE - 1; j++){

			cout << gameMap[i][j];
		}
		cout << endl;
	}
}