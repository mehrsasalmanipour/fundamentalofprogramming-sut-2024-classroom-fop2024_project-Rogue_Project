#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX 100

// Define the dungeon grid size
#define DUNGEON_WIDTH 120
#define DUNGEON_HEIGHT 40
#define FLOORS_NUM 5

#define MAX_ROOMS 10
#define MAX_FOOD_ITEMS 5

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
#define COLUMN 'O'
#define GOLD 'G'
#define BLACK_GOLD 'B'
#define TRAP 'T'
#define NORMAL_FOOD 'F'  // type 0
#define SUPER_FOOD 'A'  // type 1
#define MAGIC_FOOD 'M'  // type 2
#define MACE 'Q'
#define DAGGER 'D'
#define MAGIC_WAND 'l'
#define NORMAL_ARROW 'R'
#define SWORD 'J'
#define HEALTH_POTION 'H'
#define SPEED_POTION 'V'
#define DAMAGE_POTION 'U'
#define WINDOW '='

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
    int type;   // Type of food
    int healthEffect; // Amount of health the food restores
    int spoilTime;    // Time after which the food spoils (only relevant for normal food)
    int timeStored;   // Time passed since the food was obtained
} Food;

typedef struct {
    int type;
} Weapon;

typedef struct {
    int type;
    int healthEffect;
    int limitTime;
    int timeStored;
} Potion;

typedef struct {
    int x;
    int y;
    int color;
    int gold;
    int health;
    int hunger;
    int foodCount;
    Food foodInventory[MAX_FOOD_ITEMS];
    int weaponCount;
    int wUsed;
    Weapon weaponInventory[5];
    int potionCount;
    Potion potionInventory[100];
} Player;

// Stairs
typedef struct {
    int x, y;          // Coordinates of the staircase
    char type;         // Type of staircase ('<' for down, '>' for up)
    int floor;         // Floor where the staircase is located
} Staircase;

// extern
extern int dungeon[FLOORS_NUM][DUNGEON_HEIGHT][DUNGEON_WIDTH];
extern int copyDungeon[FLOORS_NUM][DUNGEON_HEIGHT][DUNGEON_WIDTH];
extern int currentFloor;
extern Room rooms[MAX_ROOMS];  // Declare rooms array as extern
extern int roomCount;
extern Staircase staircases[FLOORS_NUM * 2];  // Array to store staircases (2 per floor)
extern int staircaseCount;                // Counter for staircases
extern char message[100];

//// map
void initDungeon();
void carveRoom(Rect area);
void splitDungeon(Rect dungeonArea);
int manhattanDistance(Room r1, Room r2);
int find(UnionFind *uf, int x);
void unionSets(UnionFind *uf, int x, int y);
void carveCorridor(Room r1, Room r2);
void connectRooms(Room rooms[], int roomCount);
void displayDungeon(Player *player);
void displayEntireDungeon(Player *player);
int allRoomsConnected(UnionFind* uf, int roomCount);
void resetDungeon();
void copyDung();
void handleInput(Player *player, int *running);
void placeStairs();
void addColumns();
void addGold();
void addTraps();
void addFood();
void addFoodToInventory(Player *player, int type, int healthEffect, int spoilTime);
void checkFoodSpoilage(Player *player);
void eatFood(Player *player, int index);
void displayInventory(Player *player);
void addWeaponType1();
void addWeaponType2();
void addWeaponToInventory(Player *player, int type);
void displayWeaponInventory(Player *player);
void addPotion();
void addPotionToInventory(Player *player, int type);
void displayPotionInventory(Player *player);
void playerColor(Player *player);
void addWindow();

void handleGuestInput(Player *player, int *running);

void saveGame(Player *player);
void loadGame(Player *player);

// player
void placePlayerInFirstRoom(Player *player);
void movePlayer(Player *player, int newX, int newY);

#endif
