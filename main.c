#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "functions.h"

int main() {
    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
    }

    int playerIsRegistered = 0;

    while (1) {
        int mainChoice = mainMenu();
        if (mainChoice == 0) {
            clear();
            playerIsRegistered = signUp();
        } else if (mainChoice == 1) {
            clear();
            playerIsRegistered = login();
        } else if (mainChoice == 2) {

            Player player;

            while(currentFloor < FLOORS_NUM) {
                roomCount = 0;
                initDungeon();
                Rect fullDungeon = {0, DUNGEON_WIDTH, 0, DUNGEON_HEIGHT};
                splitDungeon(fullDungeon);
                connectRooms(rooms, roomCount);
                copyDung();
                if (currentFloor == 0) {
                    placePlayerInFirstRoom(&player);
                }
                currentFloor++;
            }
            currentFloor = 0;
            displayDungeon();
            int running = 1;
            while (running) {
                handleInput(&player, &running);
                refresh();
            }
        } else if (mainChoice == 3) {
            break;
        }

        if (playerIsRegistered) {
            clear();
            pregameMenu();
        }
    }

    endwin();
    return 0;
}
