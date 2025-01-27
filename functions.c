#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "functions.h"


void drawMenu(const char *menuItems[], int menuSize, int highlight) {
    clear();
    int termHight, termWidth;
    getmaxyx(stdscr, termHight, termWidth);
    int startRow = (termHight - menuSize) / 2;
    for (int i = 0; i < menuSize; i++) {
        int startCol = (termWidth - strlen(menuItems[i])) / 2;
        if (i == highlight) {
            attron(COLOR_PAIR(1));
        }
        mvprintw(startRow + i, startCol, "%s", menuItems[i]);
        attroff(COLOR_PAIR(1));
    }
    refresh();
}

int mainMenu() {
    const char *menuItems[] = {"Sign Up", "Log In", "Play as Guest", "Exit"};
    const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);
    int highlight = 0;
    int ch;

    while (1) {
        drawMenu(menuItems, menuSize, highlight);

        ch = getch();
        switch (ch) {
        case KEY_UP:
            highlight = (highlight - 1 + menuSize) % menuSize;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % menuSize;
            break;
        case 10:
            return highlight;
        }
    }
}

bool isValidPassword(const char *password) {
    if (strlen(password) < 7) {
        return false;
    }
    bool hasUpper = false, hasLower = false, hasDigit = false;
    for (int i = 0; password[i] != '\0'; i++) {
        if (isupper(password[i])) {
            hasUpper = true;
        }
        if (islower(password[i])) {
            hasLower = true;
        }
        if (isdigit(password[i])) {
            hasDigit = true;
        }
    }
    return hasUpper && hasLower && hasDigit;
}

bool isValidEmail(const char *email) {
    const char *at = strchr(email, '@');
    if (!at || at == email) {
        return false;
    }
    const char *dot = strchr(at + 1, '.');
    if (!dot || dot == at + 1) {
        return false;
    }
    if (strlen(dot + 1) == 0) {
        return false;
    }
    return true;
}

bool isUsernameTaken(const char *username) {
    char fileUsername[MAX], fileEmail[MAX], filePassword[MAX];
    FILE *file = fopen("players.txt", "r");
    if (file == NULL) {
        return false;
    }
    while (fscanf(file, "%s %s %s", fileUsername, fileEmail, filePassword) != EOF) {
        if (strcmp(username, fileUsername) == 0) {
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}

void displayError(int row, int col, const char *message) {
    attron(COLOR_PAIR(2));
    mvprintw(row, col, "Error: %s", message);
    attroff(COLOR_PAIR(2));
    refresh();
    getch();
    mvprintw(row, col, "                                         ");
    
    refresh();
}

void getInputWithDisplay(int y, int x, char *input, int maxLen) {
    int ch;
    int i = 0;

    while (1) {
        ch = getch();
        if (ch == '\n' || ch == '\r') {
            if (i == 0) {\
                attron((COLOR_PAIR(2)));
                mvprintw(y + 2, x, "Input cannot be empty!");
                attroff(COLOR_PAIR(2));
                refresh();
                getch();
                mvprintw(y + 2, x, "                       ");
                move(y, x);
                refresh();
                continue;
            }
            input[i] = '\0';
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                input[i] = '\0';
                mvaddch(y, x + i, ' ');
                move(y, x + i);
                refresh();
            }
        } else if (i < maxLen - 1 && isprint(ch)) {
            input[i++] = ch;
            mvaddch(y, x + i - 1, ch);
            refresh();
        }
    }
}

int signUp() {
    char username[MAX], email[MAX], password[MAX];
    FILE *file = fopen("players.txt", "a");
    if (file == NULL) {
        displayError(LINES - 2, 0, "Error opening file.");
        getch();
        return 0;
    }

    while (1) {
        clear();
        mvprintw(5, 10, "Enter Username (or type 'back' to return): ");
        refresh();
        getInputWithDisplay(5, 60, username, MAX);
        clrtoeol();
        if (strcmp(username, "back") == 0) {
            fclose(file);
            return 0;
        }
        if (isUsernameTaken(username)) {
            displayError(7, 10, "Username already taken!");
            getch();
            continue;
        }
        break;
    }

    while (1) {
        mvprintw(8, 10, "Enter Email (or type 'back' to return): ");
        clrtoeol();
        refresh();
        getInputWithDisplay(8, 60, email, MAX);
        if (strcmp(email, "back") == 0) {
            fclose(file);
            return 0;
        }
        if (!isValidEmail(email)) {
            displayError(10, 10, "Invalid email format!");
            getch();
            continue;
        }
        break;
    }

    while (1) {
        mvprintw(11, 10, "Enter Password (or type 'back' to return): ");
        clrtoeol();
        refresh();
        getInputWithDisplay(11, 60, password, MAX);
        if (strcmp(password, "back") == 0) {
            fclose(file);
            return 0;
        }
        if (!isValidPassword(password)) {
            displayError(13, 10, "Password must be 7+ characters with a number, uppercase and lowercase");
            getch();
            continue;
        }
        break;
    }
    fprintf(file, "%s %s %s\n", username, email, password);
    fclose(file);
    attron(COLOR_PAIR(3));
    mvprintw(15, 10, "Sign Up successful!");
    attroff(COLOR_PAIR(3));
    refresh();
    getch();

    return 1;
}

int login() {
    char username[MAX], password[MAX];
    char fileUsername[MAX], fileEmail[MAX], filePassword[MAX];
    FILE *file = fopen("players.txt", "r");
    if (file == NULL) {
        displayError(5, 10, "Error opening file.");
        getch();
        return 0;
    }

    clear();
    mvprintw(5, 10, "Enter Username: ");
    refresh();
    getInputWithDisplay(5, 26, username, MAX);

    mvprintw(7, 10, "Enter Password: ");
    refresh();
    getInputWithDisplay(7, 26, password, MAX);

    bool loginSuccess = false;
    while (fscanf(file, "%s %s %s", fileUsername, fileEmail, filePassword) != EOF) {
        if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
            loginSuccess = true;
            break;
        }
    }
    fclose(file);

    if (loginSuccess) {
        attron(COLOR_PAIR(3));
        mvprintw(9, 10, "Login successful! Welcome, %s!", username);
        attroff(COLOR_PAIR(3));
        refresh();
        getch();
        return 1;
    } else {
        displayError(9, 10, "Invalid username or password.");
        getch();
        return 0;
    }
}

void pregameMenu() {
    const char *menuItems[] = {"Save Game", "New Game", "Continue Previous Games", "Top Players", "Settings", "Back"};
    const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);
    int highlight = 0;
    int ch;

    while (1) {
        drawMenu(menuItems, menuSize, highlight);

        ch = getch();
        switch (ch) {
        case KEY_UP:
            highlight = (highlight - 1 + menuSize) % menuSize;
            break;
        case KEY_DOWN:
            highlight = (highlight + 1) % menuSize;
            break;
        case 10:
            if (highlight == 0) {
                //code
            } else if (highlight == 1) {
                GameState game;
                gameloop(&game);
            } else if (highlight == 2) {
                //code
            } else if (highlight == 3) {
                //code
            } else if (highlight == 4) {
                //code
            } else if (highlight == 5) {
                return;
            }
            break;
        }
    }
}



void initializeMap(GameState *game) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            game->tiles[y][x] = ' ';
            game->visited[y][x] = false;
        }
    }
}

int itoverlaps(GameState *game, int y, int x, int height, int width) {
    if (y + height >= MAP_HEIGHT - 1 || x + width >= MAP_WIDTH - 1) return 1;
    for (int i = y - 2; i <= height + y + 2; i++) {
        for (int j = x - 2; j <= width + x + 2; j++) {
            if (i < 0 || i >= MAP_HEIGHT || j < 0 || j >= MAP_WIDTH) continue; // Bounds check
            if (game->tiles[i][j] != ' ') return 1;
        }
    }
    return 0;
}

void generateRoom(GameState *game, int y, int x, int height, int width) {
    for (int i = y + 1; i < height + y; i++) {
        for (int j = x + 1; j < width + x; j++) {
            game->tiles[i][j] = '.';
        }
    }
    for (int i = y + 1; i <= height + y; i++) {
        game->tiles[i][x] = '|';
        game->tiles[i][x + width] = '|';
    }
    for (int j = x + 1; j < width + x; j++) {
        game->tiles[y][j] = '_';
        game->tiles[y + height][j] = '_';
    }
}



void addDoors(GameState *game, int y, int x, int height, int width) {
    int doors_placed = 0;
    int max_retries = (rand() % 3) + 1;
    int door_wall[4] = {0, 0, 0, 0};  // Tracks if a door has been placed on each wall

    while (max_retries > 0) {
        int wall = rand() % 4;
        if (door_wall[wall] == 1) {
            continue;  // Skip if there's already a door on this wall
        }

        int door_row, door_col;
        switch (wall) {
            case 0:
                door_row = y;
                door_col = x + rand() % (width);
                break;
            case 1:
                door_row = y + height;
                door_col = x + rand() % (width);
                break;
            case 2:
                door_row = y + rand() % (height);
                door_col = x;
                break;
            case 3:
                door_row = y + rand() % (height);
                door_col = x + width;
                break;
            default:
                continue;
        }

        if ((game->tiles[door_row][door_col] == '_' || game->tiles[door_row][door_col] == '|') &&
            door_row != 0 && door_row != MAP_HEIGHT && door_col != 0 && door_col != MAP_WIDTH &&
            game->tiles[door_row][door_col - 1] != '|' && game->tiles[door_row][door_col + 1] != '|' &&
            game->tiles[door_row - 1][door_col] != '_' && game->tiles[door_row + 1][door_col] != '_') {
            
            game->tiles[door_row][door_col] = '+';
            doors_placed = 1;
            door_wall[wall] = 1;  // Mark this wall as having a door
        }
        max_retries--;
    }

    if (doors_placed == 0) addDoors(game, y, x, height, width);
}



void generateCorridors(GameState *game, Room *rooms) {
    for (int i = 0; i < game->roomCount - 1; i++) {
        int x1_pos = rooms[i].x;
        int y1_pos = rooms[i].y;
        int x2_pos = rooms[i + 1].x;
        int y2_pos = rooms[i + 1].y;
        
        // Ensure doors are placed properly
        if (game->tiles[y1_pos][x1_pos] == '_' || game->tiles[y1_pos][x1_pos] == '|') {
            game->tiles[y1_pos][x1_pos] = '+';
        }
        if (game->tiles[y2_pos][x2_pos] == '_' || game->tiles[y2_pos][x2_pos] == '|') {
            game->tiles[y2_pos][x2_pos] = '+';
        }

        int currentx = x1_pos;
        int currenty = y1_pos;
        while (currentx != x2_pos) {
            currentx += (currentx < x2_pos) ? 1 : -1;
            if (game->tiles[currenty][currentx] == ' ') {
                game->tiles[currenty][currentx] = '#';
            } else if (game->tiles[currenty][currentx] != '+' &&(game->tiles[currenty][currentx] == '_' || game->tiles[currenty][currentx] == '|')) {
                game->tiles[currenty][currentx] = '+';
            }
        }
        while (currenty != y2_pos) {
            currenty += (currenty < y2_pos) ? 1 : -1;
            if (game->tiles[currenty][currentx] == ' ') {
                game->tiles[currenty][currentx] = '#';
            } else if (game->tiles[currenty][currentx] != '+' && (game->tiles[currenty][currentx] == '_' || game->tiles[currenty][currentx] == '|')) {
                game->tiles[currenty][currentx] = '+';
            }
        }
    }
}



void generate_random_map(GameState *game) {
    //srand(time(NULL));
    initializeMap(game);
    game->roomCount = rand() % (MAX_ROOMS - MIN_ROOMS + 1) + MIN_ROOMS;
    int room_index = 0;

    for (int i = 0; i < game->roomCount; i++) {
        int start_row = rand() % (MAP_HEIGHT - 9);
        int start_col = rand() % (MAP_WIDTH - 9);
        int rowscount = rand() % 5 + 5;
        int colscount = rand() % 5 + 5;
        if (!itoverlaps(game, start_row, start_col, rowscount, colscount)) {
            generateRoom(game, start_row, start_col, rowscount, colscount);
            addDoors(game, start_row, start_col, rowscount, colscount);
            game->rooms[room_index].x = start_col + colscount / 2;
            game->rooms[room_index].y = start_row + rowscount / 2;
            game->rooms[room_index].width = colscount;
            game->rooms[room_index].height = rowscount;
            room_index++;
        } else {
            i--;
        }
        if (room_index >= MAX_ROOMS) break;
    }
    generateCorridors(game, game->rooms);
}

void removeUnconnectedDoors(GameState *game) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (game->tiles[i][j] == '+') {
                int connected = 0;
                if (i > 0 && game->tiles[i - 1][j] == '#') connected = 1; // Up
                if (i < MAP_HEIGHT - 1 && game->tiles[i + 1][j] == '#') connected = 1; // Down
                if (j > 0 && game->tiles[i][j - 1] == '#') connected = 1; // Left
                if (j < MAP_WIDTH - 1 && game->tiles[i][j + 1] == '#') connected = 1; // Right

                if (!connected) {
                    // Check neighboring tiles to determine if it's a vertical or horizontal wall
                    if ((i > 0 && game->tiles[i - 1][j] == '|') || (i < MAP_HEIGHT - 1 && game->tiles[i + 1][j] == '|')) {
                        game->tiles[i][j] = '|'; // Replace with vertical wall
                    } else if ((j > 0 && game->tiles[i][j - 1] == '_') || (j < MAP_WIDTH - 1 && game->tiles[i][j + 1] == '_')) {
                        game->tiles[i][j] = '_'; // Replace with horizontal wall
                    }
                }
            }
        }
    }
}

void resetGameState(GameState *game) {
    initializeMap(game);
    game->roomCount = 0;
    // Add any other necessary reset logic here
}

void addColumnsToRooms(GameState *game) {
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        int columnCount = rand() % 2;
        for (int j = 0; j < columnCount; j++) {
            int column_x, column_y;
            bool validPosition = false;
            int retries = 10; // Maximum number of retries

            while (retries-- > 0) {
                column_x = room->x + 1 + rand() % (room->width - 2);
                column_y = room->y + 1 + rand() % (room->height - 2);

                bool inDoorline = false;
                // Check column against all door positions in the room
                for (int y = room->y; y < room->y + room->height; y++) {
                    if (game->tiles[y][column_x] == '+') {
                        inDoorline = true;
                        break;
                    }
                }
                for (int x = room->x; x < room->x + room->width; x++) {
                    if (game->tiles[column_y][x] == '+') {
                        inDoorline = true;
                        break;
                    }
                }

                if (!inDoorline && game->tiles[column_y][column_x] == '.') {
                    validPosition = true;
                    game->tiles[column_y][column_x] = 'O';
                    printf("Placed column at (%d, %d)\n", column_x, column_y);  // Debug print
                    break;
                }
            }

            if (!validPosition) {
                printf("Failed to place column in room %d after retries.\n", i);
            }
        }
    }
}

void checkMap(GameState *game) {
    // Clear the map area
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Display the correct character for each tile type
            switch (game->tiles[y][x]) {
                case '_':
                    mvprintw(y, x, "_");
                    break;
                case '.':
                    mvprintw(y, x, ".");
                    break;
                case '+':
                    mvprintw(y, x, "+");
                    break;
                case '#':
                    mvprintw(y, x, "#");
                    break;
                case '|':
                    mvprintw(y, x, "|");
                    break;
                case 'F':
                    mvprintw(y, x, "F");  // Display food tile
                    break;
                case 'P':
                    mvprintw(y, x, "P");  // Display player tile
                    break;
                case 'O':
                    mvprintw(y, x, "O");  // Display column tile
                    break;
                default:
                    mvprintw(y, x, " ");  // Display empty space
                    break;
            }
        }
    }

    // Display player's health and hunger at the bottom
    mvprintw(MAP_HEIGHT, 0, "Health: %d | Hunger: %d", game->player.health, game->player.hunger);

    refresh();  // Refresh the screen to update changes
}


void initializePlayer(GameState *game) {
    game->player.health = 100;
    game->player.hunger = 0;
    game->player.inventoryCount = 0;

    for (int roomIndex = 0; roomIndex < game->roomCount; roomIndex++) {
        Room *room = &game->rooms[roomIndex];
        
        for (int y = room->y + 1; y < room->y + room->height - 1; y++) {
            for (int x = room->x + 1; x < room->x + room->width - 1; x++) {
                if (game->tiles[y][x] == '.') {
                    game->player.x = x;
                    game->player.y = y;
                    game->tiles[y][x] = 'P';
                    return;
                }
            }
        }
    }
}

void movePlayer(GameState *game, char input) {
    int newX = game->player.x;
    int newY = game->player.y;
    static char prevTile = '.';  // Keep track of the previous tile the player was on

    // Handle directional input
    switch (input) {
        case '8': case 'j': newY -= 1; break;
        case '2': case 'k': newY += 1; break;
        case '4': case 'h': newX -= 1; break;
        case '6': case 'l': newX += 1; break;
        case '7': case 'y': newY -= 1; newX -= 1; break;
        case '9': case 'u': newY -= 1; newX += 1; break;
        case '1': case 'b': newY += 1; newX -= 1; break;
        case '3': case 'n': newY += 1; newX += 1; break;
    }

    // Validate move
    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT) {
        char nextTile = game->tiles[newY][newX];

        // Ensure player can't move through walls
        if (nextTile != '_' && nextTile != '|' && nextTile != ' ') {

            // Check for food pickup
            if (nextTile == 'F') {
                if (game->player.inventoryCount < 5) {
                    game->player.inventory[game->player.inventoryCount++] = 1;  // Add food to inventory
                    mvprintw(MAP_HEIGHT + 1, 0, "Picked up food. Inventory now has %d items.", game->player.inventoryCount);
                    nextTile = '.';  // Clear the food tile if picked up
                } else {
                    mvprintw(MAP_HEIGHT + 1, 0, "Inventory full! Cannot pick up more food.");
                }
            }

            // Restore the previous tile if it was a food tile and the inventory was full
            if (prevTile == 'F') {
                game->tiles[game->player.y][game->player.x] = 'F';
            } else {
                game->tiles[game->player.y][game->player.x] = prevTile;
            }

            // Update player position
            prevTile = nextTile;  // Store the current tile before moving
            game->player.x = newX;
            game->player.y = newY;
            game->tiles[newY][newX] = 'P';  // Place the player on the new tile

            mvprintw(MAP_HEIGHT + 2, 0, "Player moved to (%d, %d)", game->player.x, game->player.y);
        }
    }

    refresh();  // Refresh the screen to update changes
}


void addFoodToMap(GameState *game) {
    for (int i = 1; i < game->roomCount - 1; i++) {  // Start from room 2 to the room before the last
        Room *room = &game->rooms[i];
        int foodCount = rand() % 3;  // At most 2 food items per room (0, 1, or 2)

        for (int j = 0; j < foodCount; j++) {
            int x, y;
            bool placed = false;
            int retries = 100;  // Maximum number of retries per food item

            while (retries-- > 0) {
                x = room->x + 1 + rand() % (room->width - 2);  // Ensure food is placed within room boundaries
                y = room->y + 1 + rand() % (room->height - 2);

                if (game->tiles[y][x] == '.') {
                    game->tiles[y][x] = 'F';
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                mvprintw(MAP_HEIGHT + 5, 0, "Failed to place food item in room %d after retries.", i);
            }
        }
    }
}

void displayInventory(GameState *game) {
    int pos = 0;  // Current position in the inventory

    while (true) {
        clear();  // Clear the screen to display the inventory

        mvprintw(0, 0, "Player Inventory:");
        for (int i = 0; i < game->player.inventoryCount; i++) {
            if (i == pos) {
                mvprintw(i + 1, 0, "> Food Item %d", i + 1);  // Highlight the selected item
            } else {
                mvprintw(i + 1, 0, "  Food Item %d", i + 1);
            }
        }

        if (game->player.inventoryCount == 0) {
            mvprintw(1, 0, "Inventory is empty.");
        }

        mvprintw(game->player.inventoryCount + 2, 0, "Use arrow keys to navigate and Enter to select.");
        mvprintw(game->player.inventoryCount + 3, 0, "Press any other key to return.");
        refresh();  // Refresh the screen to show the inventory

        int input = getch();  // Wait for the player to press a key

        if (input == KEY_UP) {  // Handle up arrow key
            if (pos > 0) {
                pos--;
            }
        } else if (input == KEY_DOWN) {  // Handle down arrow key
            if (pos < game->player.inventoryCount - 1) {
                pos++;
            }
        } else if (input == '\n') {  // Handle Enter key
            if (pos < game->player.inventoryCount) {
                eatFood(game, pos);  // Eat the selected food
                break;  // Exit the inventory menu after eating
            }
        } else {
            break;  // Exit the inventory menu on any other key
        }
    }
}

void eatFood(GameState *game, int index) {
    if (index < game->player.inventoryCount) {
        reduceHunger(game, 20);  // Reduce hunger by 20
        increaseHealth(game, 10);  // Increase health by 10

        // Remove the eaten food from the inventory
        for (int i = index; i < game->player.inventoryCount - 1; i++) {
            game->player.inventory[i] = game->player.inventory[i + 1];
        }
        game->player.inventoryCount--;

        refresh();
    }
}

void reduceHunger(GameState *game, int amount) {
    game->player.hunger -= amount;
    if (game->player.hunger < 0) {
        game->player.hunger = 0;  // Ensure hunger doesn't go below 0
    }
    mvprintw(MAP_HEIGHT, 0, "Health: %d | Hunger: %d", game->player.health, game->player.hunger);
    refresh();  // Refresh the screen to update changes
}

void increaseHealth(GameState *game, int amount) {
    game->player.health += amount;
    if (game->player.health > 100) {
        game->player.health = 100;  // Ensure health doesn't go above 100
    }
    mvprintw(MAP_HEIGHT, 0, "Health: %d | Hunger: %d", game->player.health, game->player.hunger);
    refresh();  // Refresh the screen to update changes
}

void increaseHunger(GameState *game) {
    game->player.hunger += 10;  // Increment hunger by 10
    if (game->player.hunger > 100) {
        game->player.hunger = 100;  // Cap at 100
    }
    mvprintw(MAP_HEIGHT, 0, "Health: %d | Hunger: %d", game->player.health, game->player.hunger);

    // If hunger reaches 100, decrease health
    if (game->player.hunger == 100) {
        decreasePlayerHealth(game);
    }
    refresh();  // Refresh the screen to update changes
}

void decreasePlayerHealth(GameState *game) {
    if (game->player.health > 0) {
        game->player.health -= 10;  // Decrement health by 10
        if (game->player.health < 0) {
            game->player.health = 0;  // Ensure health doesn't go below 0
        }
        mvprintw(MAP_HEIGHT, 0, "Health: %d | Hunger: %d", game->player.health, game->player.hunger);
        refresh();  // Refresh the screen to update changes
    }
}

void gameloop(GameState *game) {
    resetGameState(game);
    generate_random_map(game);
    removeUnconnectedDoors(game);
    addColumnsToRooms(game);
    addFoodToMap(game);  // Ensure food is added to the map
    initializePlayer(game);

    clear();  // Clear the screen before entering the game loop
    refresh();  // Refresh to apply changes

    char input;
    int turnCount = 0;

    while (true) {
        // Clear and redraw the game map
        clear();
        checkMap(game);

        // Increase hunger every 10 moves
        if (turnCount > 0 && turnCount % 10 == 0) {
            increaseHunger(game);
        }

        input = getch();  // Get player input

        if (input == 'x') {
            clear();  // Clear the screen when exiting the game loop
            break;  // Exit the game loop on 'x'
        } else if (input == 'e') {
            displayInventory(game);  // Display inventory on 'e'
        } else {
            movePlayer(game, input);  // Handle player movement
            turnCount++;  // Increment turn count after a move
        }

        refresh();  // Refresh the screen to update changes
    }
}
