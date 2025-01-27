#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define MAX 100
#define MAP_WIDTH 50
#define MAP_HEIGHT 30
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

typedef enum {
    NORMAL_FOOD,
    HEALING_FOOD,
    MAGIC_FOOD,
    POISONOUS_FOOD
} FoodType;

typedef struct {
    FoodType type;
    int hungerEffect;
    int healthEffect;
    char name[20];
    int x;
    int y;
} Food;

typedef struct {
    int x;
    int y;
    int health;
    int hunger;
    int inventoryCount;
    int inventory[5];
} Player;

typedef struct {
    int x, y, height, width;
} Room;

typedef struct {
    char tiles[MAP_HEIGHT][MAP_WIDTH];
    bool visited[MAP_HEIGHT][MAP_WIDTH];
    Room rooms[MAX_ROOMS];
    int roomCount;
    Player player;
} GameState;

void initializeMap(GameState *game);
int itoverlaps(GameState *game, int y, int x, int height, int width);
void generateRoom(GameState *game, int y, int x, int height, int width);
void addDoors(GameState *game, int y, int x, int height, int width);
void generateCorridors(GameState *game, Room *rooms);
void removeUnconnectedDoors(GameState *game);
void generate_random_map(GameState *game);
void resetGameState(GameState *game);
void checkMap(GameState *game);
void addColumnsToRooms(GameState *game);
void initializePlayer(GameState *game);
void movePlayer(GameState *game, char input);
void addFoodToMap(GameState *game);
void displayInventory(GameState *game);
void eatFood(GameState *game, int index);
void reduceHunger(GameState *game, int amount);
void increaseHealth(GameState *game, int amount);
void increaseHunger(GameState *game);
void decreasePlayerHealth(GameState *game);
void gameloop(GameState *game);

#endif
