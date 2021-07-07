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
	Direction direction;
} Ghost;

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
const Direction dirs[] = { UP, RIGHT, DOWN, LEFT };
Ghost ghosts[4] = { 0 };
HANDLE hConsole;
PacMan player;
int mapSize;
void print(char* s, Color c);
int coinFlip(void);
void addOther();
Ghost makeGhosts(int x, int y);
void printMap(void);
void updateFrame(void);
void manageMove(void);
int move(void);
void moveGhosts(void);
int writeAnsi(PCWSTR sequence);
void up(void);
void down(void);
void left(void);
void right(void);
void pause(void);
int checkMap(void);
void exitGame(void);
int getIndex(Direction direction);


void main(void) {
	srand((unsigned int)time(NULL));
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	player.score = 0;
	player.x = 11;
	player.y = 11;
	player.direction = UP;
	player.symbol = _UP;
	mapSize = 12;
	int g = 0;
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			ghosts[g] = makeGhosts(6 + x, 6 + y);
			g++;
		}
	}

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

void addOther(void) {
	/*Add secondary identifiers to map grid*/
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (mapGrid[i][j] == PATH && coinFlip() == 1 && coinFlip() == 1) {
				mapGrid[i][j] = FOOD;
			}
		}
	}
}

Ghost makeGhosts(int x, int y) {
	Ghost temp;
	temp.x = x;
	temp.y = y;
	temp.direction = UP;
	return temp;
}

void printMap(void) {
	/*Print out the map*/
	printf("Score: %i\n\n", player.score);
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			int printed = FALSE;
			if (player.x - 1 == j && player.y - 1 == i) {
				printf("%s", dirSymbols[player.symbol]);
				printed = TRUE;
			}
			for (int g = 0; g < 4; g++) {
				Ghost gh = ghosts[g];
				if (gh.x - 1 == j && gh.y - 1 == i) {
					printf("%i", gh.y);
					printed = TRUE;
				}
			}
			if (!printed) {
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
		if (!move()) break;
		moveGhosts();
		pause();
	}
	exitGame();
}

int move(void) {
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
	if (nextBlock == WALL) {
		player.x = old.x;
		player.y = old.y;
		return TRUE;
	}

	if (nextBlock == FOOD) {
		player.score += 10;
		mapGrid[player.y - 1][player.x - 1] = PATH;
		if (checkMap()) addOther();
	}

	for (int g = 0; g < 4; g++) {
		Ghost gh = ghosts[g];
		if (gh.x == player.x && gh.y == player.y) {
			return FALSE;
		}
	}

	return TRUE;
}

void moveGhosts(void) {
	for (int g = 0; g < 4; g++) {
		Ghost gh = ghosts[g];
		Coordinate old;
		old.x = gh.x;
		old.y = gh.y;
		switch (gh.direction) {
		case UP:
			if (gh.y > 1) {
				gh.y--;
			}
			break;
		case RIGHT:
			if (gh.x < mapSize - 1) {
				gh.x++;
			}
			break;
		case DOWN:
			if (gh.y < mapSize - 1) {
				gh.y++;
			}
			break;
		case LEFT:
			if (gh.x > 1) {
				gh.x--;
			}
			break;
		}
		if (mapGrid[gh.y - 1][gh.x - 1] == WALL) {
			gh.x = old.x;
			gh.y = old.y;
			int index = getIndex(gh.direction);
			if (coinFlip()) index++;
			else index += 3;
			if (index > 3) index -= 4;
		}
	}
}

int writeAnsi(PCWSTR sequence) {
	/*Executes ansi sequences on windows. Copied from StackOverflow*/
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

int checkMap(void) {
	for (int i = 0; i < mapSize; i++) {
		for (int j = 0; j < mapSize; j++) {
			if (mapGrid[i][j] == FOOD) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

void exitGame(void) {
	system("cls");
	printf("Oh No You Lost!\nYour score was %i\n", player.score);
}

int getIndex(Direction direction) {
	for (int g = 0; g < 4; g++) {
		if (direction == dirs[g]) {
			return g;
		}
	}
	return -1;
}