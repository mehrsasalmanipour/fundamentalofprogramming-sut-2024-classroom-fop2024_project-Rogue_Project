#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX 100

// Define the dungeon grid size
#define DUNGEON_WIDTH 120
#define DUNGEON_HEIGHT 40
#define FLOORS_NUM 2

#define MAX_ROOMS 10

// Cell types
#define WALL 1
#define FLOOR 0
#define CORRIDOR '#'
#define DOOR '+'
#define WALL_H '_'
#define WALL_V '|'
#define PLAYER 'P'
#define DOWN_STAIR '<'
#define UP_STAIR '>'

// menu and login
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

// structs

// Structure for a rectangle (sub dungeon)
typedef struct {
    int x_min, x_max, y_min, y_max;
} Rect;

typedef struct {
    int x_min, x_max, y_min, y_max;
    int index;
} Room;

// Union Find structure to manage room connections
typedef struct {
    int *parent;
    int *rank;
    int n;
} UnionFind;

UnionFind* createUnionFind(int n);

typedef struct {
    int x;
    int y;
    // Other player properties like health, name, etc.
} Player;

//typedef enum {
//    NORMAL_FOOD,
//    HEALING_FOOD,
//    MAGIC_FOOD,
//    POISONOUS_FOOD
//} FoodType;
//
//typedef struct {
//    FoodType type;
//    int hungerEffect;
//    int healthEffect;
//    char name[20];
//    int x;
//    int y;
//} Food;

// extern
extern int dungeon[FLOORS_NUM][DUNGEON_HEIGHT][DUNGEON_WIDTH];
extern int copyDungeon[FLOORS_NUM][DUNGEON_HEIGHT][DUNGEON_WIDTH];
extern int currentFloor;
extern Room rooms[MAX_ROOMS];  // Declare rooms array as extern
extern int roomCount;

//// map
void initDungeon();
void carveRoom(Rect area);
void splitDungeon(Rect dungeonArea);
int manhattanDistance(Room r1, Room r2);
int find(UnionFind *uf, int x);
void unionSets(UnionFind *uf, int x, int y);
void carveCorridor(Room r1, Room r2);
void connectRooms(Room rooms[], int roomCount);
void displayDungeon();
int allRoomsConnected(UnionFind* uf, int roomCount);
void resetDungeon();
void copyDung();
void movePlayer(Player *player, int newX, int newY);
void handleInput(Player *player, int *running);

// player
void placePlayerInFirstRoom(Player *player);

/* void addColumnsToRooms(GameState *game);
void initializePlayer(GameState *game);
void movePlayer(GameState *game, char input);
void addFoodToMap(GameState *game);
void displayInventory(GameState *game);
void eatFood(GameState *game, int index);
void reduceHunger(GameState *game, int amount);
void increaseHealth(GameState *game, int amount);
void increaseHunger(GameState *game);
void decreasePlayerHealth(GameState *game); */


#endif
