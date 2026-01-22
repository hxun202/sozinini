#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>

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

void MovePlayer(Player* p) {
	if (GetAsyncKeyState(VK_UP) & 0x8000)    p->y--;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)  p->y++;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)  p->x--;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) p->x++;
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

void Boss(Monster* sergius)
{
	if(sergius->hp <= sergius->maxhp / 2 && sergius->phase ==1)
	{
		sergius->attack *= 1.5;
		sergius->defense *= 1.3;
		sergius->phase = 2;
		printf("세르기우스가 분노합니다. . .\n");
	}
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

void rest(Player* player)
{
	player->hp = player->maxhp;
	player->mp = player->maxmp;
	printf("휴식을 통해 기력을 되찾았습니다.\n");
}

int main()
{

	GetConsoleScreenBufferInfo(hStdout, &console);

	Player player = { 10, 5 };
	
	int x = 0;
	int y = 0;

	Initialize();

	Render(x, y, "☆");

	while (1) 
	{
		Flip();
		Clear();

		MovePlayer(&player);

		if (_kbhit()) {
			char key = _getch();
		}

		Render(x, y, "☆");
	}



	return 0;
}