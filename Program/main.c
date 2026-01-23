#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define MAP_W 40
#define MAP_H 20

#define SCREEN_W 150
#define SCREEN_H 40

char screen[SCREEN_H][SCREEN_W + 1];

int px = 2, py = 2;

typedef struct {
    int level;
    int hp;
    int maxHp;
    int attack;
	int defense;
    int mp;
	int maxMp;
} Player;

Player player = { 1, 100, 100, 30, 20, 50, 50 };

char map[MAP_H][MAP_W + 1] = {
    "########################################",
    "#......................................#",
    "#..######...............######.........#",
    "#......................................#",
    "#..............####....................#",
    "#......................................#",
    "#....####...............................#",
    "#......................................#",
    "#.........................######.......#",
    "#......................................#",
    "#..............####....................#",
    "#......................................#",
    "#....####...............................#",
    "#......................................#",
    "#.........................######.......#",
    "#......................................#",
    "#..............####....................#",
    "#..................................B...#",
    "#......................................#",
    "########################################"
};

void initConsole();
void clearBuffer();
void render();
void drawMap();
void drawStatus();
void drawMessage(const char* msg);
void movePlayer();

int main()
{
    initConsole();

    while (1)
    {
        clearBuffer();
        drawMap();
        drawStatus();
        drawMessage("방향키로 이동");
        render();

        movePlayer();

        if (map[py][px] == 'B')
        {
            clearBuffer();
            drawMessage("세르기우스를 발견했다!");
            render();
            _getch();
            break;
        }
    }
    return 0;
}

void initConsole()
{
    system("mode con cols=160 lines=45");
    CONSOLE_CURSOR_INFO cci = { 1, FALSE };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

void clearBuffer()
{
    for (int y = 0; y < SCREEN_H; y++)
    {
        for (int x = 0; x < SCREEN_W; x++)
            screen[y][x] = ' ';
        screen[y][SCREEN_W] = '\0';
    }
}

// ===== 맵 그리기 =====
void drawMap()
{
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            screen[y][x] = map[y][x];
}

void drawStatus()
{
    sprintf(&screen[1][50], "========== STATUS ==========");
    sprintf(&screen[3][50], "이벨");
    sprintf(&screen[4][50], "LV : %d", player.level);
    sprintf(&screen[5][50], "HP : %d / %d", player.hp, player.maxHp);
    sprintf(&screen[6][50], "MP : %d / %d", player.mp, player.maxMp);
    sprintf(&screen[7][50], "ATTACK : %d", player.attack);
    sprintf(&screen[8][50], "DEFENSE : %d", player.defense);
}

void drawMessage(const char* msg)
{
    sprintf(&screen[25][0], "------------------------------------------------------------");
    sprintf(&screen[26][0], "%s", msg);
    sprintf(&screen[27][0], "------------------------------------------------------------");
}

// ===== 화면 출력 =====
void render()
{
    system("cls");
    for (int y = 0; y < SCREEN_H; y++)
        printf("%s\n", screen[y]);
}

// ===== 이동 =====
void movePlayer()
{
    int key = _getch();
    if (key == 224 || key == 0)
        key = _getch();

    int nx = px;
    int ny = py;

    if (key == 72) ny--;
    if (key == 80) ny++;
    if (key == 75) nx--;
    if (key == 77) nx++;

    if (map[ny][nx] != '#')
    {
        map[py][px] = '.';
        px = nx;
        py = ny;
        map[py][px] = '@';
    }
}
