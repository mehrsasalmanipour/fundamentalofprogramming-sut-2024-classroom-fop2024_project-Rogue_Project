#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX 100
#define MAP_WIDTH 50
#define MAP_HEIGHT 20
#define ROOM_MIN_SIZE 4
#define ROOM_MAX_SIZE 8
#define MIN_ROOMS 6
#define MAX_ROOMS 8

void drawMenu(const char *menuItems[], int menuSize, int highlight);
int mainMenu();
bool isValidPassword(const char *password);
bool isValidEmail(const char *email);
bool isUsernameTaken(const char *username);
void displayError(int row, int col, const char *message);
void getInputWithDisplay(int y, int x, char *input, int maxLen);
int signUp();
int login();
void pregameMenu();
typedef struct {
    int x, y, width, height;
    int doorsX[4];
    int doorsY[4];
    int doorCount;
} Room;
typedef struct {
    char tiles[MAP_HEIGHT][MAP_WIDTH];
    bool visited[MAP_HEIGHT][MAP_WIDTH];
    Room rooms[MAX_ROOMS];
    int roomCount;
    int playerX, playerY;
} GameState;
void initializeMap(GameState *game);
bool isOverlapping(Room *a, Room *b);
void generateRooms(GameState *game);
void addDoors(GameState *game);
void generateCorridors(GameState *game);
void checkMap(GameState *game);

#endif
