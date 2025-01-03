#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "functions.h"

#define MAX 100
#define MAP_WIDTH 50
#define MAP_HEIGHT 20
#define ROOM_MIN_SIZE 4
#define ROOM_MAX_SIZE 8
#define MIN_ROOMS 6
#define MAX_ROOMS 8

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
                initializeMap(&game);
                generateRooms(&game);
                addDoors(&game);
                generateCorridors(&game);
                checkMap(&game);
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

bool isOverlapping(Room *a, Room *b) {
    return !(a->x + a->width < b->x || b->x + b->width < a->x || a->y + a->height < b->y || b->y + b->height < a->y);
}

void generateRooms(GameState *game) {
    game->roomCount = 0;
    while (game->roomCount < MAX_ROOMS) {
        int width = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int height = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int x = rand() % (MAP_WIDTH - width - 1) + 1;
        int y = rand() % (MAP_HEIGHT - height - 1) + 1;

        Room newRoom = {x, y, width, height, {0}, {0}, 0};
        bool overlaps = false;

        for (int i = 0; i < game->roomCount; i++) {
            if (isOverlapping(&newRoom, &game->rooms[i])) {
                overlaps = true;
                break;
            }
        }
        if (!overlaps) {
            game->rooms[game->roomCount++] = newRoom;
            for (int j = y; j < y + height; j++) {
                for (int k = x; k < x + width; k++) {
                    if (j == y || j == y + height - 1) {
                        game->tiles[j][k] = '-';
                    } else if (k == x || k == x + width - 1) {
                        game->tiles[j][k] = '|';
                    } else {
                        game->tiles[j][k] = '.';
                    }
                }
            }
        }
    }
    // Debug: Print room details
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        printf("Room %d: (%d, %d), Size: %d x %d\n", i + 1, room->x, room->y, room->width, room->height);
    }
}

void addDoors(GameState *game) {
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        int numDoors = rand() % 3 + 1;

        for (int j = 0; j < numDoors; j++) {
            int wall = rand() % 4;
            int doorX = 0, doorY = 0;
            bool validDoor = false;

            while (!validDoor) {
                if (wall == 0 && room->y > 1) {
                    doorX = rand() % (room->width - 2) + room->x + 1;
                    doorY = room->y;
                    validDoor = true;
                } else if (wall == 1 && room->y + room->height < MAP_HEIGHT - 1) {
                    doorX = rand() % (room->width - 2) + room->x + 1;
                    doorY = room->y + room->height - 1;
                    validDoor = true;
                } else if (wall == 2 && room->x > 1) {
                    doorX = room->x;
                    doorY = rand() % (room->height - 2) + room->y + 1;
                    validDoor = true;
                } else if (wall == 3 && room->x + room->width < MAP_WIDTH - 1) {
                    doorX = room->x + room->width - 1;
                    doorY = rand() % (room->height - 2) + room->y + 1;
                    validDoor = true;
                } else {
                    wall = rand() % 4;
                }
            }
            
            room->doorsX[room->doorCount] = doorX;
            room->doorsY[room->doorCount] = doorY;
            room->doorCount++;
            game->tiles[doorY][doorX] = '+';
        }
    }

    // Debug: Print door details
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        for (int j = 0; j < room->doorCount; j++) {
            printf("Room %d Door %d: (%d, %d)\n", i + 1, j + 1, room->doorsX[j], room->doorsY[j]);
        }
    }
}

void generateCorridors(GameState *game) {
    for (int i = 1; i < game->roomCount; i++) {
        Room *currentRoom = &game->rooms[i - 1];
        Room *nextRoom = &game->rooms[i];

        int currentDoorIndex = rand() % currentRoom->doorCount;
        int currentDoorX = currentRoom->doorsX[currentDoorIndex];
        int currentDoorY = currentRoom->doorsY[currentDoorIndex];

        int nextDoorIndex = rand() % nextRoom->doorCount;
        int nextDoorX = nextRoom->doorsX[nextDoorIndex];
        int nextDoorY = nextRoom->doorsY[nextDoorIndex];

        int cx = currentDoorX, cy = currentDoorY;

        // Debug: Print corridor generation details
        printf("Generating corridor from (%d, %d) to (%d, %d)\n", currentDoorX, currentDoorY, nextDoorX, nextDoorY);

        // Generate a horizontal corridor
        while (cx != nextDoorX) {
            if (cx < nextDoorX) {
                cx++;
            } else {
                cx--;
            }
            if (game->tiles[cy][cx] == ' ') {
                game->tiles[cy][cx] = '#';
            }
        }

        // Generate a vertical corridor
        while (cy != nextDoorY) {
            if (cy < nextDoorY) {
                cy++;
            } else {
                cy--;
            }
            if (game->tiles[cy][cx] == ' ') {
                game->tiles[cy][cx] = '#';
            }
        }

        // Ensure doors are properly marked
        game->tiles[currentDoorY][currentDoorX] = '+';
        game->tiles[nextDoorY][nextDoorX] = '+';
    }
}

void checkMap(GameState *game) {
    clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            mvaddch(y, x, game->tiles[y][x]);
        }
    }
    mvprintw(MAP_HEIGHT + 1, 0, "Total Rooms: %d", game->roomCount);
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        mvprintw(MAP_HEIGHT + 2 + i, 0, "Room %d: (%d, %d), Size: %d x %d", i + 1, room->x, room->y, room->width, room->height);
    }
    refresh();
    getch();
}

/*void initializeMap(GameState *game) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            game->tiles[y][x] = ' ';
            game->visited[y][x] = false;
        }
    }
}

bool isOverlapping(Room *a, Room *b) {
    return !(a->x + a->width < b->x || b->x + b->width < a->x || a->y + a->height < b->y || b->y + b->height < a->y);
}

void generateRooms(GameState *game) {
    game->roomCount = 0;
    while (game->roomCount < MAX_ROOMS) {
        int width = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int height = rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1) + ROOM_MIN_SIZE;
        int x = rand() % (MAP_WIDTH - width - 1) + 1;
        int y = rand() % (MAP_HEIGHT - height - 1) + 1;

        Room newRoom = {x, y, width, height, {0}, {0}, 0};
        bool overlaps = false;

        for (int i = 0; i < game->roomCount; i++) {
            if (isOverlapping(&newRoom, &game->rooms[i])) {
                overlaps = true;
                break;
            }
        }
        if (!overlaps) {
            game->rooms[game->roomCount++] = newRoom;
            for (int j = y; j < y + height; j++) {
                for (int k = x; k < x + width; k++) {
                    if (j == y || j == y + height - 1) {
                        game->tiles[j][k] = '-';
                    } else if (k == x || k == x + width - 1) {
                        game->tiles[j][k] = '|';
                    } else {
                        game->tiles[j][k] = '.';
                    }
                }
            }
        }
    }
}

void addDoors(GameState *game) {
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        int numDoors = rand() % 3 + 1;

        for (int j = 0; j < numDoors; j++) {
            int wall = rand() % 4;
            int doorX = 0, doorY = 0;
            bool validDoor = false;

            while (!validDoor) {
                if (wall == 0 && room->y > 1) {
                    doorX = rand() % (room->width - 2) + room->x + 1;
                    doorY = room->y;
                    validDoor = true;
                } else if (wall == 1 && room->y + room->height < MAP_HEIGHT - 1) {
                    doorX = rand() % (room->width - 2) + room->x + 1;
                    doorY = room->y + room->height - 1;
                    validDoor = true;
                } else if (wall == 2 && room->x > 1) {
                    doorX = room->x;
                    doorY = rand() % (room->height - 2) + room->y + 1;
                    validDoor = true;
                } else if (wall == 3 && room->x + room->width < MAP_WIDTH - 1) {
                    doorX = room->x + room->width - 1;
                    doorY = rand() % (room->height - 2) + room->y + 1;
                } else {
                    wall = rand() % 4;
                }
            }
            
            room->doorsX[room->doorCount] = doorX;
            room->doorsY[room->doorCount] = doorY;
            room->doorCount++;
            game->tiles[doorY][doorX] = '+';
        }
    }
}

void generateCorridors(GameState *game) {
    for (int i = 1; i < game->roomCount; i++) {
        Room *currentRoom = &game->rooms[i - 1];
        Room *nextRoom = &game->rooms[i];

        int currentDoorIndex = rand() % currentRoom->doorCount;
        int currentDoorX = currentRoom->doorsX[currentDoorIndex];
        int currentDoorY = currentRoom->doorsY[currentDoorIndex];

        int nextDoorIndex = rand() % nextRoom->doorCount;
        int nextDoorX = nextRoom->doorsX[nextDoorIndex];
        int nextDoorY = nextRoom->doorsY[nextDoorIndex];

        int cx = currentDoorX, cy = currentDoorY;

        // Generate a horizontal corridor
        while (cx != nextDoorX) {
            if (cx < nextDoorX) {
                cx++;
            } else {
                cx--;
            }
            if (game->tiles[cy][cx] == ' ') {
                game->tiles[cy][cx] = '#';
            }
        }

        // Generate a vertical corridor
        while (cy != nextDoorY) {
            if (cy < nextDoorY) {
                cy++;
            } else {
                cy--;
            }
            if (game->tiles[cy][cx] == ' ') {
                game->tiles[cy][cx] = '#';
            }
        }
    }
}



void checkMap(GameState *game) {
    clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            mvaddch(y, x, game->tiles[y][x]);
        }
    }
    mvprintw(MAP_HEIGHT + 1, 0, "Total Rooms: %d", game->roomCount);
    for (int i = 0; i < game->roomCount; i++) {
        Room *room = &game->rooms[i];
        mvprintw(MAP_HEIGHT + 2 + i, 0, "Room %d: (%d, %d), Size: %d x %d", i + 1, room->x, room->y, room->width, room->height);
    }
    refresh();
    getch();
}*/
