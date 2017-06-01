#include "GamePlayInfo.h"


GamePlayInfo::GamePlayInfo()
{
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