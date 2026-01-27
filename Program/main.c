#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

#define MAP_W 40
#define MAP_H 20
#define BATTLE_W 80
#define BATTLE_H 25

#define SCREEN_W 150
#define SCREEN_H 40

#define LOG_LINES 4

char logs[LOG_LINES][64];
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
    int maxHp;
    int attack;
    int defense;
} Boss; 

typedef enum
{
    WIN,
    LOSE
}Result;

typedef enum
{
    STATE_MENU,
    STATE_MAP,
    STATE_BATTLE,
    STATE_EXIT
} GameState;

GameState gameState = STATE_MENU;
Player player = { 1, 100, 100, 30, 20, 50, 50 };
Boss boss = { 300, 300, 50, 40 };

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
void drawMenu();
void drawStatus();
void drawMessage(const char* msg);
void drawText(int x, int y, int width, const char* text);
void movePlayer();
void check();
void pushlog(const char* msg);
Result Resultbattle(Boss*boss);
void BattleScreen(Boss* boss, const char* msg);

int main()
{
    initConsole();
    map[py][px] = '@';

    while (1)
    {
        if (gameState == STATE_MENU)
        {
            drawMenu();
            char select = _getch();

            if (select == '1')
                gameState = STATE_MAP;
            else if (select == '2')
            {
                clearBuffer();
                drawMessage("설정은 아직 구현되지 않았습니다.");
                render();
                _getch();
            }
            else if (select == '3')
                break;
            continue;
		}
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

void drawMenu()
{
    clearBuffer();
    drawText(10, 50, 20, "=== MAIN MENU ===");
    drawText(12, 50, 20, "1. 게임 시작");
    drawText(13, 50, 20, "2. 설정");
    drawText(14, 50, 20, "3. 종료");
    render();
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
    sprintf(&screen[15][50], "ESC를 눌러 메인 메뉴로 돌아갈 수 있습니다.");
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

    if (key == 27)
    {
        gameState = STATE_MENU;
        return;
    }

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

		Boss boss = { 300, 300, 80, 50 };

        Result result = Resultbattle(&boss);

        if (result == WIN)
        {
            clearBuffer();
            pushlog("세르기우스를 물리쳤습니다!");
            nextTile = 0;
        }
        else
        {
            clearBuffer();
            drawMessage
            ("세르기우스에게 패배했습니다...\n"
                "1. 다시 싸우기\n"
                "2. 마을로 돌아가기\n");
            render();

            char select = _getch();

            if (select == '1')
            {
                Boss newBoss = { 350, 80, 50 };
                player.hp = player.maxHp;
                player.mp = player.maxMp;
                Resultbattle(&newBoss);
            }
            else
            {
                player.hp = player.maxHp;
                player.mp = player.maxMp;

                px = 2;
                py = 2;
                map[py][px] = '@';
				nextTile = 0;
            }
        }

    }
}

void pushlog(const char* msg)
{
    for (int i = 0; i < LOG_LINES - 1; i++)
        strcpy(logs[i], logs[i + 1]);

    strncpy(logs[LOG_LINES - 1], msg, 63);
    logs[LOG_LINES - 1][63] = '\0';
}

void BattleScreen(Boss* boss, const char* msg)
{
    int left = (SCREEN_W - BATTLE_W) / 2;
    int top = (SCREEN_H - BATTLE_H) / 2;

    // 테두리
    for (int x = 0; x < BATTLE_W; x++)
    {
        screen[top][left + x] = '#';
        screen[top + BATTLE_H - 1][left + x] = '#';
    }
    for (int y = 0; y < BATTLE_H; y++)
    {
        screen[top + y][left] = '#';
        screen[top + y][left + BATTLE_W - 1] = '#';
    }

    // 제목
    drawText(top + 1, left + 30, 20, "=== BATTLE ===");

    // 플레이어
    drawText(top + 3, left + 5, 20, "이벨");
    snprintf(&screen[top + 4][left + 5], 30, "HP : %d / %d", player.hp, player.maxHp);
    snprintf(&screen[top + 5][left + 5], 30, "MP : %d / %d", player.mp, player.maxMp);

    // 보스 
    drawText(top + 3, left + 55, 20, "세르기우스");
    char hpBuf[32];
    snprintf(hpBuf, sizeof(hpBuf),
        "HP : %d / %d", boss->hp, boss->maxHp);
    drawText(top + 4, left + 55, 30, hpBuf);



    // 스킬
    drawText(top + 8, left + 5, 40, "Q: 별의 폭발 (-5 MP)");
    drawText(top + 9, left + 5, 40, "W: 빛의 장막 (-10 MP)");
    drawText(top + 10, left + 5, 40, "E: 성운의 일격 (-15 MP)");
    drawText(top + 11, left + 5, 40, "R: 운명의 공명 (-35 MP)");

    // 로그 박스
    drawText(top + 17, left + 5, 50, "----------------------------------------------");
    for (int i = 0; i < LOG_LINES; i++)
        drawText(top + 18 + i, left + 5, 50, logs[i]);
    drawText(top + 22, left + 5, 50, "----------------------------------------------");
}


Result Resultbattle(Boss* boss)
{
    char key;

    Turn turn = TURN_PLAYER;

    for (int i = 0; i < LOG_LINES; i++)
        strcpy(logs[i], "");

    pushlog("세르기우스와의 전투가 시작됩니다.");

    while (boss->hp > 0 && player.hp > 0)
    {
        clearBuffer();

        BattleScreen(boss, turn == TURN_PLAYER ? "당신의 턴입니다." : "세르기우스의 턴입니다.");

        render();

        if (turn == TURN_PLAYER)
        {
            key = _getch();

            int acted = 1;

            if (key == 'q' || key == 'Q')
            {
                if (player.mp >= 5)
                {
                    player.mp -= 5;
                    boss->hp -= player.attack * 2;
                    pushlog("세르기우스에게 60의 피해를 입혔습니다.");
                }
                else
                {
                    pushlog("MP가 부족합니다.");
                    acted = 0;
                }
            }
            else if (key == 'w' || key == 'W')
            {
                if (player.mp >= 10)
                {
                    player.mp -= 10;
                    player.hp += 5;
                    player.mp += 20;
                    pushlog("보호막을 생성해 5의 체력과 20MP를 회복했습니다.");
                }
                else
                {
                    pushlog("MP가 부족합니다.");
                    acted = 0;
                }
            }
            else if (key == 'e' || key == 'E')
            {
                if (player.mp >= 15)
                {
                    player.mp -= 15;
                    boss->hp -= player.attack * 3;
                    pushlog("세르기우스에게 90의 피해를 입혔습니다!");
                }
                else
                {
                    pushlog("MP가 부족합니다.");
                    acted = 0;
                }
            }
            else if (key == 'r' || key == 'R')
            {
                if (player.mp >= 35)
                {
                    player.mp -= 35;
                    boss->hp -= player.attack * 5;
                    pushlog("세르기우스에게 150의 피해를 입혔습니다!");
                }
                else
                {
                    pushlog("MP가 부족합니다.");
                    acted = 0;
                }
            }
            else
            {
                pushlog("잘못된 키입니다. Q W E R 중에서 선택하세요.");
                acted = 0;
            }

            if (acted)
            {
                turn = TURN_BOSS;
				pushlog("세르기우스의 턴입니다.");
            }
        }
        else if (turn == TURN_BOSS)
        {
            int damage = boss->attack / 2 - player.defense;
            if (damage < 5) damage = 5;
            player.hp -= damage;

            char buffer[64];

            snprintf(buffer, sizeof(buffer), " 세르기우스의 공격으로 %d의 피해를 입었습니다.", damage);
            pushlog(buffer);

            _getch();
            turn = TURN_PLAYER;
        }

        if (boss->hp <= 0) return WIN;
        if (player.hp <= 0) return LOSE;
    }
    return LOSE;
}