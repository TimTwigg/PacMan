# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <time.h>
# include <windows.h>
# include <conio.h>

typedef enum {
	UP = 72,
	RIGHT = 77,
	DOWN = 80,
	LEFT = 75,
} Direction;

typedef enum {
	PATH = 0,
	WALL = 1,
	FOOD = 2,
} MapGridKey;

typedef enum {
	STD = 7,
	BLUE_DARK = 1,
	BLUE_LIGHT = 3,
	CYAN = 11,
	GREEN_DARK = 2,
	GREEN_LIGHT = 10,
	RED_DARK = 4,
	RED_LIGHT = 12,
	YELLOW = 6,
	PURPLE = 13,
	GRAY = 8,
} Color;

typedef enum {
	LINE_UP = 0,
	LINE_DOWN = 1,
	CURSOR_RIGHT = 2,
	CURSOR_LEFT = 3,
	CLEAR_SCREEN = 4,
	CURSOR_TOP_LEFT = 5,
	CLEAR_TO_EOL = 6,
} ANSI_INDEX;

typedef enum {
	_UP = 1,
	_RIGHT = 2,
	_DOWN = 3,
	_LEFT = 4,
} SYMBOL_INDEX;

typedef struct {
	int score;
	int x;
	int y;
	Direction direction;
	int symbol;
} PacMan;

typedef struct {
	int x;
	int y;
} Coordinate;

int mapGrid[12][12] = {
	1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,1,
	1,0,1,1,0,1,1,0,1,1,0,1,
	1,0,1,0,0,0,0,0,0,1,0,1,
	1,0,1,0,1,0,0,1,0,0,0,1,
	1,0,1,0,1,0,0,1,0,1,0,1,
	1,0,0,0,1,0,0,1,0,1,0,1,
	1,0,1,0,1,1,1,1,0,1,0,1,
	1,0,1,0,0,0,0,0,0,1,0,1,
	1,0,1,1,1,1,1,0,1,1,0,1,
	1,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1
};
const PCWSTR ansiCodes[] = { L"\x1B[A", L"\x1B[B", L"\x1B[C", L"\x1B[D", L"\x1B[2J", L"\x1B[H", L"\x1B[K", };
const char* dirSymbols[] = { "", "^", ">", "v", "<" };
HANDLE hConsole;
PacMan player;
int mapSize;
void print(char* s, Color c);
int coinFlip(void);
void addOther();
void printMap();
void updateFrame(void);
void manageMove(void);
void move(void);
int writeAnsi(PCWSTR sequence);
void up(void);
void down(void);
void left(void);
void right(void);
void pause(void);


void main(void) {
	srand((unsigned int)time(NULL));
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	player.score = 0;
	player.x = 11;
	player.y = 11;
	player.direction = UP;
	player.symbol = _UP;
	mapSize = 12;

	puts("Welcome to PacMan!");
	addOther();
	printMap();
	manageMove();
}

void print(char* s, Color c) {
	SetConsoleTextAttribute(hConsole, c);
	printf("%s", s);
	SetConsoleTextAttribute(hConsole, STD);
}

int coinFlip(void) {
	int n = rand();
	if (n % 2 == 0) {
		return 0;
	}
	else {
		return 1;
	}
}

void addOther() {
	/*Add secondary identifiers to map grid*/
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (mapGrid[i][j] == PATH && coinFlip() == 1 && coinFlip() == 1) {
				mapGrid[i][j] = FOOD;
			}
		}
	}
}

void printMap() {
	/*Print out the map*/
	printf("Score: %i\n\n", player.score);
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (player.x - 1 == j && player.y - 1 == i) {
				printf("%s", dirSymbols[player.symbol]);
			}
			else {
				int id = mapGrid[i][j];
				switch (id) {
				case PATH:
					printf(" ");
					break;
				case WALL:
					print("+", GRAY);
					break;
				case FOOD:
					print("*", CYAN);
					break;
				}
			}
			printf(" ");
		}
		puts("");
	}
}

void updateFrame(void) {
	writeAnsi(L"\033[2;1H");
	printMap();
}

void manageMove(void) {
	while (1) {
		if (_kbhit()) {
			int key = _getch();
			if (key == UP && player.direction != DOWN) {
				player.direction = UP;
				player.symbol = _UP;
			}
			else if (key == RIGHT && player.direction != LEFT) {
				player.direction = RIGHT;
				player.symbol = _RIGHT;
			}
			else if (key == DOWN && player.direction != UP) {
				player.direction = DOWN;
				player.symbol = _DOWN;
			}
			else if (key == LEFT && player.direction != RIGHT) {
				player.direction = LEFT;
				player.symbol = _LEFT;
			}
			else if (key == 27) {
				return;
			}
			else {
				continue;
			}
		}
		move();
		pause();
	}
}

void move(void) {
	updateFrame();
	int nextBlock = -1;
	writeAnsi(L"\033[16;1H");
	Coordinate old;
	old.x = player.x;
	old.y = player.y;
	switch (player.direction) {
	case UP:
		if (player.y > 1) {
			player.y--;
		}
		break;
	case RIGHT:
		if (player.x < mapSize - 1) {
			player.x++;
		}
		break;
	case DOWN:
		if (player.y < mapSize - 1) {
			player.y++;
		}
		break;
	case LEFT:
		if (player.x > 1) {
			player.x--;
		}
		break;
	}
	nextBlock = mapGrid[player.y - 1][player.x - 1];
	printf("\nCoords: %i %i  \n", player.x, player.y);
	if (nextBlock == WALL) {
		/*TODO find other direction or just stop ? */
		player.x = old.x;
		player.y = old.y;
		return;
	}

	/*TODO move ghosts*/

	if (nextBlock == FOOD) {
		player.score += 10;
		mapGrid[player.y - 1][player.x - 1] = PATH;
	}

	/*TODO die if meet ghost*/
}

int writeAnsi(PCWSTR sequence) {
	DWORD mode;
	if (!GetConsoleMode(hConsole, &mode)) {
		return GetLastError();
	}
	const DWORD originalMode = mode;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hConsole, mode)) {
		return GetLastError();
	}
	DWORD written;
	if (!WriteConsoleW(hConsole, sequence, (DWORD)wcslen(sequence), &written, NULL)) {
		SetConsoleMode(hConsole, originalMode);
		return GetLastError();
	}
	SetConsoleMode(hConsole, originalMode);
	printf("\r");
	return 0;
}

void up(void) {
	writeAnsi(ansiCodes[LINE_UP]);
}
void down(void) {
	writeAnsi(ansiCodes[LINE_DOWN]);
}
void left(void) {
	writeAnsi(ansiCodes[CURSOR_LEFT]);
}
void right(void) {
	writeAnsi(ansiCodes[CURSOR_RIGHT]);
}

void pause(void) {
	Sleep(500);
}