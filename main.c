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
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_CYAN, COLOR_BLACK);
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
            staircaseCount = 0;
            Player player;
            player.color = 0;
            player.gold = 0;
            player.health = 100;
            player.hunger = 0;
            player.foodCount = 0;
            player.weaponCount = 0;
            player.potionCount = 0;
            player.wUsed = 0;

            while(currentFloor < FLOORS_NUM) {
                roomCount = 0;
                // Initialize dungeon with walls
                initDungeon();
                // Define the entire dungeon area to split (from (0,0) to (DUNGEON_WIDTH, DUNGEON_HEIGHT))
                Rect fullDungeon = {0, DUNGEON_WIDTH, 0, DUNGEON_HEIGHT};
                // Split the dungeon recursively
                splitDungeon(fullDungeon);

                connectRooms(rooms, roomCount);

                addWindow();

                placeStairs();

                addColumns();

                addFood();

                if (currentFloor == 0) {
                    addWeaponType1();
                }

                if (currentFloor == 1) {
                    addWeaponType2();
                }

                copyDung();

                if (currentFloor == 0) {
                    placePlayerInFirstRoom(&player);
                }

                addGold();

                addMonster();

                addTraps();

                addPotion();

                currentFloor++;
            }

            currentFloor = 0;

            displayDungeon(&player);

            int running = 1;
            while (running) {
                if (player.health <= 0) {
                    clear();
                    mvprintw(1, 1, "You Lost!");
                    getch();
                    break;
                }
                if (player.gold >= 50) {
                    clear();
                    mvprintw(1, 1, "You Won!");
                    getch();
                    break;
                }
                clear();  // Clear the screen first
                mvprintw(DUNGEON_HEIGHT, 0, "Gold: %d | Health: %d", player.gold, player.health);
                mvprintw(DUNGEON_HEIGHT + 2, 0, "Message: %s", message);
                displayDungeon(&player);  // Display the dungeon
                refresh();  // Refresh the screen to show the updates
                handleGuestInput(&player, &running);  // Handle player input
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
