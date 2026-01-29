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
    int exp;
    int nextexp;
} Player;

typedef struct
{
    char name[32];
    int hp;
    int maxHp;
    int attack;
    int defense;
    int exp;
} Monster; 

typedef enum
{
    BOSS,
    SLIME,
    GOBLIN,
    MON_COUNT
};
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
} GameState;

int bossCleared = 0;
GameState gameState = STATE_MENU;
Player player = { 1, 100, 100, 25, 10, 50, 50, 0, 100 };
Monster monster[MON_COUNT] =
{
    { "세르기우스", 500, 500, 80, 60, 200 },
    { "슬라임", 30, 30, 5, 5, 15 },
    { "고블린", 60, 60, 15, 5, 30 }
};


char map[MAP_H][MAP_W + 1] = {
    "########################################",
    "#..............................S.......#",
    "#..######..S............######....S....#",
    "#........G.............................#",
    "#....G.........####.......G............#",
    "#....................S.............G...#",
    "#....####.......S..............G.......#",
    "#............S............G............#",
    "#..S..................S...######.......#",
    "#.....S.....................S..........#",
    "#...........G..####..............G.....#",
    "#.............S.................G......#",
    "#....####...........S..................#",
    "#.......G...............S..............#",
    "#..............G..........######.......#",
    "#......S...............................#",
    "#..........S...####.........S..........#",
    "#.........G.............S..........B...#",
    "#...................G..........S.......#",
    "########################################"
};

void initConsole();
void clearBuffer();
void render();
void drawMap();
void drawMenu();
void drawStatus();
void drawMessage(const char* msg);
void printSlow();
void drawText(int x, int y, int width, const char* text);
void movePlayer();
void levelUp();
void endingStory();
void openingStory();
void check();
void pushlog(const char* msg);
Result Resultbattle(Monster*monster);
void BattleScreen(Monster* monster);

int main()
{
    initConsole();

    openingStory();

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
                system("pause");
            }
            else if (select == '3')
            {
                player.hp = player.maxHp;
                player.mp = player.maxMp;

                clearBuffer();
                drawMessage("휴식을 취합니다.\n"
                    "HP와 MP가 모두 회복되었습니다.");
                render();
                system("pause");
            }
            else if (select == '4')
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

        if (bossCleared)
        {
            endingStory();
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
    drawText(14, 50, 20, "3. 휴식");
    drawText(15, 50, 20, "4. 종료");
    render();
}

void drawStatus()
{
    sprintf(&screen[1][50], "========== STATUS ==========");
    sprintf(&screen[3][50], "이벨");
    sprintf(&screen[4][50], "LV : %d", player.level);
    sprintf(&screen[5][50], "EXP : %d / %d", player.exp, player.nextexp);
    sprintf(&screen[6][50], "HP : %d / %d", player.hp, player.maxHp);
    sprintf(&screen[7][50], "MP : %d / %d", player.mp, player.maxMp);
    sprintf(&screen[8][50], "ATTACK : %d", player.attack);
    sprintf(&screen[9][50], "DEFENSE : %d", player.defense);
    sprintf(&screen[15][50], "ESC를 눌러 메인 메뉴로 돌아갈 수 있습니다.");
}

void drawMessage(const char* msg)
{
    sprintf(&screen[25][0], "------------------------------------------------------------");
    sprintf(&screen[26][0], "%s", msg);
    sprintf(&screen[27][0], "------------------------------------------------------------");
}

void printSlow(const char* text, int delay)
{
    for (int i = 0; text[i] != '\0'; i++)
    {
        printf("%c", text[i]);
        fflush(stdout);
        Sleep(delay);
    }
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
        if (nextTile == 'B')
        {
            map[px][py] = '@';
        }
        else
        {
            map[py][px] = '@';
        }
    }
}

void levelUp()
{
    while (player.exp >= player.nextexp)
    {
        player.exp -= player.nextexp;
        player.level++;

        player.maxHp += 20;
        player.maxMp += 10;
        player.attack += 8;
        player.defense += 5;

        player.hp = player.maxHp;
        player.mp = player.maxMp;

        player.nextexp += 20;

        pushlog("레벨 업!");
    }
}

void openingStory()
{
    system("cls");

    printSlow("\n'검은 별 아래 태어날 자, 어둠의 군주를 무너뜨릴 것이다. 하나 그의 손에는 피가 가득하리니.'\n\n", 60);
    Sleep(500);

    printSlow("오래 전부터 존재해온 예언과 마법의 나라 에테르니아.\n", 60);
    Sleep(500);

    printSlow("평화로운 하루가 계속되던 어느 날\n", 60);
    Sleep(500);

    printSlow("암흑군이 에테르니아를 공격하기 시작한다.\n", 60);
    Sleep(500);

    printSlow("예언의 주인공이라 지목되는\n\n", 60);
    Sleep(500);

    printSlow("'이벨'\n\n", 60);
    Sleep(500);

    printSlow("이벨은 과연 어둠의 군주를 쓰러트릴 수 있을까 . . .\n\n\n\n", 60);
    Sleep(500);

    printSlow("에테르니아를 지켜라!\n", 60);
    Sleep(500);

    printSlow("START\n", 60);
    Sleep(500);

    system("pause");
}

void endingStory()
{
    system("cls");

    printSlow("\n세르기우스를 쓰러트리자\n", 60);
    Sleep(500);

    printSlow("비로소 암흑군과의 전쟁이 막을 내렸다.\n", 60);
    Sleep(500);

    printSlow("이벨의 손에 힘이 풀리고...\n", 60);
    Sleep(500);

    printSlow("검이 바닥에 닿는 소리가 울려 퍼진다.\n", 60);
    Sleep(500);

    printSlow("이벨이 하늘을 올려다 보았고\n", 60);
    Sleep(500);

    printSlow("또 다른 예언이 시작될 예감이 들었다.\n", 60);
    Sleep(500);

    printSlow("앞으로 이벨은 어떤 운명을 맞이할 것인가...\n\n", 60);
    Sleep(30+rand()%40);

    printSlow("The End\n", 60);
    Sleep(500);

    system("pause");
    exit(0);
}

Monster* getMonsterByTile(char tile)
{
    switch (tile)
    {
    case 'B': return &monster[BOSS];
    case 'G': return &monster[GOBLIN];
    case 'S': return &monster[SLIME];
    default: return NULL;
    }
}


void check()
{
    Monster* m = getMonsterByTile(nextTile);
    if (!m) return;

    clearBuffer();

    char msg[128];
    snprintf(msg, sizeof(msg), "%s를 발견했습니다!\n%s와의 전투를 시작합니다.\n준비되면 아무 키나 눌러 전투를 시작하세요.", m->name, m->name);
    drawMessage(msg);
    render();
    _getch();

    Monster temp = *m;  
    Result result = Resultbattle(&temp);

        if (result == WIN)
        {
            player.exp += temp.exp;

            levelUp();

            if (m == &monster[BOSS])
            {
                bossCleared = 1;
                endingStory();
                return;
            }

            map[py][px] = '.';
            nextTile = 0;

            clearBuffer();
            snprintf(msg, sizeof(msg), "%s를 물리쳤습니다!\n아무 키나 눌러 맵으로 돌아가세요.", m->name);
            drawMessage(msg);
            render();
            _getch();
            return;
        }
        else
        {
            clearBuffer();
            drawMessage
            ("패배했습니다...\n"
                "1. 다시 싸우기\n"
                "2. 마을로 돌아가기\n");
            render();

            map[px][py] = 'B';
            px = 2;
            py = 2;
            map[px][py] = '@';

            char select = _getch();

            if (select == '1')
            {
                Monster newBoss = { "세르기우스", 500, 500, 80, 60, 200};
                player.hp = player.maxHp;
                player.mp = player.maxMp;
         
                Result retryResult = Resultbattle(&newBoss);

                if (retryResult == WIN)
                {
                    player.exp += newBoss.exp;
                    levelUp();

                    clearBuffer();
                    drawMessage("세르기우스를 쓰러뜨렸습니다!");
                    render();
                    _getch();
                }
                else
                {
                    clearBuffer();
                    drawMessage("세르기우스에게 패배했습니다...");
                    render();
                    _getch();
                }

                nextTile = 0;
            }
            else
            {
                player.hp = player.maxHp;
                player.mp = player.maxMp;

                map[py][px] = '@';
				nextTile = 0;
            }
        }
        nextTile = 0;
}

void pushlog(const char* msg)
{
    for (int i = 0; i < LOG_LINES - 1; i++)
        strcpy(logs[i], logs[i + 1]);

    strncpy(logs[LOG_LINES - 1], msg, 63);
    logs[LOG_LINES - 1][63] = '\0';
}

int turncount = 1;
void BattleScreen(Monster* monster)
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

    char buf[32];
    snprintf(buf, sizeof(buf), "%d턴", turncount);
    drawText(top + 8, left + 62, 20, buf);

    // 제목
    drawText(top + 1, left + 30, 20, "=== BATTLE ===");

    // 플레이어
    drawText(top + 3, left + 5, 20, "이벨");
    char hpBuf[32];
    sprintf(hpBuf, "HP : %d / %d", player.hp, player.maxHp);
    drawText(top + 4, left + 5, 30, hpBuf);
    snprintf(&screen[top + 5][left + 5], 30, "MP : %d / %d", player.mp, player.maxMp);

    // 보스 
    drawText(top + 3, left + 60, 20, monster->name);
    sprintf(hpBuf, "HP : %d / %d", monster->hp, monster->maxHp);
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


Result Resultbattle(Monster* monster)
{
    char key;

    Turn turn = TURN_PLAYER;

    for (int i = 0; i < LOG_LINES; i++)
        strcpy(logs[i], "");

    char buf[64];
    snprintf(buf, sizeof(buf), "%s와의 전투가 시작됩니다.", monster->name);
    pushlog(buf);

    while (monster->hp > 0 && player.hp > 0)
    {
        clearBuffer();

        BattleScreen(monster);

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
                    monster->hp -= player.attack * 1.5;
                    int damage = player.attack * 1.5;
                    snprintf(buf, sizeof(buf), "%s에게 %d의 피해를 입혔습니다.", monster->name, damage);
                    pushlog(buf);
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
                    monster->hp -= player.attack * 3;
                    int damage = player.attack * 3;
                    snprintf(buf, sizeof(buf), "%s에게 %d의 피해를 입혔습니다.", monster->name, damage);
                    pushlog(buf);
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
                    monster->hp -= player.attack * 5;
                    int damage = player.attack * 5;
                    snprintf(buf, sizeof(buf), "%s에게 %d의 피해를 입혔습니다.", monster->name, damage);
                    pushlog(buf);
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
                snprintf(buf, sizeof(buf), "%s의 턴입니다.", monster->name);
                pushlog(buf);
            }
        }
        else if (turn == TURN_BOSS)
        {
            int damage = monster->attack / 2 - player.defense;
            if (damage < 5) damage = 5;
            player.hp -= damage;

            char buffer[64];

            snprintf(buffer, sizeof(buffer), "%s의 공격으로 %d의 피해를 입었습니다.", monster->name, damage);
            pushlog(buffer);

            _getch();
            turn = TURN_PLAYER;
            turncount++;
        }

        if (monster->hp <= 0) return WIN;
        if (player.hp <= 0) return LOSE;
    }
    return LOSE;
}