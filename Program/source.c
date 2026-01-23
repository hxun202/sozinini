#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include "map.h"

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

int screenIndex;
HANDLE screen[2];

typedef struct
{
	int x;
	int y;
	int hp;
	int maxhp;
	int mp;
	int maxmp;
	int attack;
	int defense;
	int xp;
	int level;
} Player;

typedef struct
{
	int hp;
	int maxhp;
	int attack;
	int defense;
	int rewardXp;
	int phase;
} Monster;

typedef enum
{
	Accept,
	Refect
} Choice;

#pragma region 화면
void Initialize()
{
	CONSOLE_CURSOR_INFO cursor;

	screen[0] = CreateConsoleScreenBuffer
	(
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
	);

	screen[1] = CreateConsoleScreenBuffer
	(
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
	);

	cursor.bVisible = FALSE;

	SetConsoleCursorInfo(screen[0], &cursor);
	SetConsoleCursorInfo(screen[1], &cursor);
}

void Flip()
{
	SetConsoleActiveScreenBuffer(screen[screenIndex]);

	screenIndex = !screenIndex;
}

void Clear()
{
	COORD position = { 0, 0 };

	DWORD dword;

	CONSOLE_SCREEN_BUFFER_INFO buffer;

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(console, &buffer);

	int width = buffer.srWindow.Right - buffer.srWindow.Left + 1;
	int height = buffer.srWindow.Bottom - buffer.srWindow.Top + 1;

	FillConsoleOutputCharacter
	(
		screen[screenIndex], ' ', width * height, position, &dword
	);
}

void Release()
{
	CloseHandle(screen[0]);
	CloseHandle(screen[1]);
}

void Render(int x, int y, const char* character)
{
	DWORD dword;
	COORD position = { x, y };

	SetConsoleCursorPosition(screen[screenIndex], position);
	WriteFile(screen[screenIndex], character, strlen(character), &dword, NULL);
}
#pragma endregion

#pragma region 이벨
void MovePlayer(Player* p) {
	char key = 0;

	CONSOLE_SCREEN_BUFFER_INFO console;

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hStdout, &console);

	int width = console.srWindow.Right - console.srWindow.Left - 2;
	int height = console.srWindow.Bottom - console.srWindow.Top;

	int x = 0;
	int y = 0;

	Initialize();

	Render(x, y, "☆");

	while (1)
	{
		Flip();

		Clear();

		key = _getch();

		if (key == -32 || key == 0)
		{
			key = _getch();
		}

		switch (key)
		{
		case UP: if (y > 0) { y--; }
			   break;

		case LEFT: if (x > 0) { x -= 2; }
				 break;

		case RIGHT: if (width > x) { x += 2; }
				  break;

		case DOWN: if (height > y) { y++; }
				 break;

		default:
			break;
		}

		Render(x, y, "☆");
	}
}

void  PlayerStatus(Player* player)
{
	char buffer[100];
	printf(buffer, "HP: %d/%d  MP: %d/%d  XP: %d  Level: %d",
		player->hp, player->maxhp,
		player->mp, player->maxmp,
		player->xp,
		player->level);
	Render(0, 0, buffer);
}

void levelup(Player* player)
{
	player->level += 1;
	player->maxhp += 20;
	player->maxmp += 10;
	player->attack += 10;
	player->defense += 3;

	printf("Level Up\n");
	printf("현재 레벨: %d\n", player->level);
}

void skill(char key)
{
	switch (key)
	{
	case 'q' :
	case 'Q': printf("별의 폭발!\n");
		break;

	case 'w':
	case 'W': printf("빛의 장막 발동\n");
		break;

	case 'e':
	case 'E': printf("성운의 일격!\n");
		break;

	case 'r':
	case 'R': printf("운명이 공명한다!!\n");
		break;
	}
}

void rest(Player* player)
{
	player->hp = player->maxhp;
	player->mp = player->maxmp;
	printf("휴식을 통해 기력을 되찾았습니다.\n");
}

#pragma endregion

#pragma region 세르기우스
void Boss(Monster* sergius)
{
	if (sergius->hp <= sergius->maxhp / 2 && sergius->phase == 1)
	{
		sergius->attack *= 1.5;
		sergius->defense *= 1.3;
		sergius->phase = 2;
		printf("세르기우스가 분노합니다. . .\n");
	}
}
#pragma endregion

#pragma region  맵

Map* LoadMap(const char* filename) {
	FILE* f = fopen(filename, "r");
	if (!f) return NULL;

	// 예: 파일 첫 줄에 "width height\n" 형식이라 가정
	int width = 0, height = 0;
	
	if (width <= 0 || height <= 0) { fclose(f); return NULL; }

	Map* map = malloc(sizeof(Map));
	if (!map) { fclose(f); return NULL; }
	map->width = width;
	map->height = height;

	map->data = malloc(sizeof(char*) * map->height);
	if (!map->data) { free(map); fclose(f); return NULL; }

	char* linebuf = malloc(map->width + 2);
	if (!linebuf) {
		free(map->data); free(map); fclose(f); return NULL;
	}

	for (int y = 0; y < map->height; y++) {
		if (!fgets(linebuf, map->width + 2, f)) { // 읽기 실패
			// 정리
			for (int i = 0; i < y; i++) free(map->data[i]);
			free(linebuf); free(map->data); free(map); fclose(f);
			return NULL;
		}
		// 줄바꿈 제거 및 널종단 보장
		size_t len = strcspn(linebuf, "\r\n");
		map->data[y] = malloc(map->width + 1);
		if (!map->data[y]) {
			for (int i = 0; i < y; i++) free(map->data[i]);
			free(linebuf); free(map->data); free(map); fclose(f);
			return NULL;
		}
		// 필요한 경우 패딩으로 채움
		memset(map->data[y], ' ', map->width);
		memcpy(map->data[y], linebuf, len < (size_t)map->width ? len : map->width);
		map->data[y][map->width] = '\0';
		// consume potential extra newline already handled by fgets
	}

	free(linebuf);
	fclose(f);
	return map;
}

char GetTile(Map* map, int x, int y) {
	if (!map || !map->data) return '\0';
	if (x < 0 || x >= map->width || y < 0 || y >= map->height) return '\0';
	return map->data[y][x];
}

void FreeMap(Map* map) {
	if (!map) return;
	if (map->data) {
		for (int y = 0; y < map->height; y++) free(map->data[y]);
		free(map->data);
	}
	free(map);
}
#pragma endregion

void Battle(Player* player, Monster* monster)
{
	printf("세르기우스 : 예언이 시작되겠네. 즐거운 싸움을 해보자.\n");

	while (player->hp > 0 && monster->hp > 0)
	{
		Monster Sergius = { 250, 250, 80, 80, 100, 2 };

		int choice;

		printf("\n이벨\nhp : %d/%d mp : %d/%d\n", player->hp, player->maxhp, player->mp, player->maxmp);
		printf("\n세르기우스 hp : %d/%d\n", monster->hp, monster->maxhp);
		printf("");

		monster->hp -= player->attack;

		if (monster->hp <= 0)
		{
			player->xp += monster->rewardXp;
			break;
		}
		else(player->hp <= 0);
		{
			printf("Game Over. . .");
			exit(0);
		}
		player->hp -= monster->attack;
	}
}

int main()
{
	 MovePlayer(NULL);

	LoadMap("Resources/test.txt");
	/*if (map == NULL) {
		printf("맵 파일을 불러올 수 없습니다.\n");
		return 1;
	}*/

	Player player;
	player.x = 1;
	player.y = 1;
	player.hp = 100;

	
	/*while (1) 
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000) 
		{
			if (GetTile(map, player.x, player.y - 1) != '#')
				player.y--;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			if (GetTile(map, player.x, player.y + 1) != '#')
				player.y++;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			if (GetTile(map, player.x - 1, player.y) != '#')
				player.x--;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			if (GetTile(map, player.x + 1, player.y) != '#')
				player.x++;
		}*/

		if (_kbhit()) 
		{
			char key = _getch();
			if (key == 'q' || key == 'Q')
				printf("Q 스킬 발동!\n");
			if (key == 'w' || key == 'W')
				printf("W 스킬 발동!\n");
			if (key == 'e' || key == 'E')
				printf("E 스킬 발동!\n");
			if (key == 'r' || key == 'R')
				printf("궁극기 발동!\n");
		}

	/*	system("cls");
		for (int y = 0; y < map->height; y++) 
		{
			for (int x = 0; x < map->width; x++)
			{
				if (x == player.x && y == player.y)
					printf("@");
				else
					printf("%c", map->data[y][x]);
			}
			printf("\n");
		}
	}*/

	//FreeMap(map);

	return 0;
}