#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "functions.h"

int main() {
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
        int choice = mainMenu();
        if (choice == 0) {
            clear();
            signUp();
        } else if (choice == 1) {
            clear();
            login();
        } else if (choice == 2) {
            //code
        } else if (choice == 3) {
            break;
        }
    }

    endwin();
    return 0;
}
