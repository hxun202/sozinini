#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define MAP_W 40
#define MAP_H 20
#define BATTLE_W 80
#define BATTLE_H 25

#define SCREEN_W 150
#define SCREEN_H 40

char screen[SCREEN_H][SCREEN_W + 1];

int px = 2, py = 2;

typedef enum
{
	TURN_PLAYER,
	TURN_BOSS
}Turn;

typedef struct {
    int level;
    int hp;
    int maxHp;
    int attack;
	int defense;
    int mp;
	int maxMp;
} Player;

typedef struct
{
    int hp;
    int attack;
    int defense;
} Boss; 

Player player = { 1, 100, 100, 30, 20, 50, 50 };
Boss boss = { 350, 80, 50 };

char map[MAP_H][MAP_W + 1] = {
    "########################################",
    "#......................................#",
    "#..######...............######.........#",
    "#......................................#",
    "#..............####....................#",
    "#......................................#",
    "#....####..............................#",
    "#......................................#",
    "#.........................######.......#",
    "#......................................#",
    "#..............####....................#",
    "#......................................#",
    "#....####..............................#",
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
void drawText(int x, int y, int width, const char* text);
void movePlayer();
void check();
void battle();
void BattleScreen(Boss* boss, const char* msg);

int main()
{
    initConsole();
    map[py][px] = '@';

    while (1)
    {
        clearBuffer();
        drawMap();
        drawStatus();
        drawMessage("방향키로 이동");
        render();

        movePlayer();
        check();
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

void drawText(int y, int x, int width, const char* text)
{
    for (int i = 0; i < width && text[i]; i++)
        screen[y][x + i] = text[i];
}

void render()
{
    system("cls");
    for (int y = 0; y < SCREEN_H; y++)
        printf("%s\n", screen[y]);
}

char nextTile = 0;
void movePlayer()
{
    char key = _getch();

    int nx = px;
    int ny = py;

    if (key == 72) ny--;
    if (key == 80) ny++;
    if (key == 75) nx--;
    if (key == 77) nx++;

    nextTile = map[ny][nx];

    if (nextTile!= '#')
    {
        map[py][px] = '.';
        px = nx;
        py = ny;
        map[py][px] = '@';
    }
}

void check()
{
    if (nextTile == 'B')
    {
        clearBuffer();
        drawMessage
        ( "세르기우스를 발견했습니다!\n"
        "세르기우스와의 전투를 시작합니다.\n"
	    "준비되면 아무 키나 눌러 전투를 시작하세요.");
        render();
        _getch();

		Boss boss = { 350, 80, 50 };
        battle(&boss);

        if (boss.hp <= 0)
        {
            drawMessage("세르기우스를 물리쳤습니다!");
            render();
            system("pause");
            exit(0);
        }
        if (player.hp <= 0)
        {
            clearBuffer();
            BattleScreen(&boss, "세르기우스와의 전투에서 패배했습니다...");
            render();
            _getch();
            exit(0);
        }
    }
}

void BattleScreen(Boss* boss, const char* msg)
{
    int left = (SCREEN_W - 80) / 2;  
    int top = (SCREEN_H - 25) / 2;

    for (int x = 0; x < 80; x++)
    {
        screen[top][left + x] = '#';
        screen[top + 24][left + x] = '#';
    }
    for (int y = 0; y < 25; y++)
    {
        screen[top + y][left] = '#';
        screen[top + y][left + 79] = '#';
    }

    snprintf(&screen[top + 1][left + 30], SCREEN_W, "=== BATTLE ===");

    // 플레이어
    snprintf(&screen[top + 3][left + 5], SCREEN_W, "이벨");
    snprintf(&screen[top + 4][left + 5], SCREEN_W, "HP : %d / %d", player.hp, player.maxHp);
    snprintf(&screen[top + 5][left + 5], SCREEN_W, "MP : %d / %d", player.mp, player.maxMp);

    // 보스
    snprintf(&screen[top + 6][left + 60], SCREEN_W, "세르기우스");
    snprintf(&screen[top + 7][left + 60], SCREEN_W, "HP : %d", boss->hp);

    // 스킬
    snprintf(&screen[top + 8][left + 5], SCREEN_W, "Q: 별의 폭발 (-10 MP)");
    snprintf(&screen[top + 9][left + 5], SCREEN_W, "W: 빛의 장막 (-5 MP)");
    snprintf(&screen[top + 10][left + 5], SCREEN_W, "E: 성운의 일격 (-15 MP)");
    snprintf(&screen[top + 11][left + 5], SCREEN_W, "R: 운명의 공명 (-30 MP)");

    // 메시지 박스
    drawText(top + 19, left + 5, BATTLE_W - 10,
        "----------------------------------------------");
    drawText(top + 20, left + 5, BATTLE_W - 10, msg);
    drawText(top + 21, left + 5, BATTLE_W - 10,
        "----------------------------------------------");
}

void battle(Boss* boss)
{
    char key;
    char message[256] = "당신의 턴입니다.";
    Turn turn = TURN_PLAYER;

    while (boss->hp > 0 && player.hp > 0)
    {
        clearBuffer();

        if (turn == TURN_BOSS)
         snprintf(message, sizeof(message), "세르기우스의 턴입니다.");

        BattleScreen(boss, message);

        render();

        if (turn == TURN_PLAYER)
        {
            key = _getch();

            int acted = 0;

            if (key == 'q' || key == 'Q')
            {
                if (player.mp >= 5)
                {
                    player.mp -= 5;
                    boss->hp -= player.attack * 2;
                    snprintf(message, sizeof(message), "세르기우스에게 %d의 피해를 입혔습니다.", player.attack * 2);
                }
                else
                {
                    snprintf(message, sizeof(message), "MP가 부족합니다.");
                }
            }
            else if (key == 'w' || key == 'W')
            {
                if (player.mp >= 10)
                {
                    player.mp -= 10;
                    player.hp += 5;
                    snprintf(message, sizeof(message), "보호막을 생성해 5의 체력을 회복했습니다.");
                }
                else
                {
                    snprintf(message, sizeof(message), "MP가 부족합니다.");
                }
            }
            else if (key == 'e' || key == 'E')
            {
                if (player.mp >= 15)
                {
                    player.mp -= 15;
                    boss->hp -= player.attack * 3;
                    snprintf(message, sizeof(message), "세르기우스에게 %d의 피해를 입혔습니다!", player.attack * 3);
                }
                else
                {
                    snprintf(message, sizeof(message), "MP가 부족합니다.");
                }
            }
            else if (key == 'r' || key == 'R')
            {
                if (player.mp >= 35)
                {
                    player.mp -= 35;
                    boss->hp -= player.attack * 5;
                    snprintf(message, sizeof(message), "운명의 공명! 세르기우스에게 %d의 피해를 입혔습니다!", player.attack * 5);
                }
                else
                {
                    snprintf(message, sizeof(message), "MP가 부족합니다.");
                }
                if (acted)
                {
                    acted = 1;
                    turn = TURN_BOSS;
                }
                else
                {
                    int damage = boss->attack - player.defense;
                    if (damage < 1) damage = 1;
                    player.hp -= damage;
                    snprintf(message, sizeof(message), " 세르기우스의 공격으로 %d의 피해를 입었습니다.", damage);
                }
                render();
                _getch();
                turn = TURN_PLAYER;
            }
        }
    }
}