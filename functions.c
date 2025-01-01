#include <ncurses.h>
#include <string.h>
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
    const char *menuItems[] = {"Sign Up", "Log In", "Top Players", "New Game", "Continue" "Settings", "Exit"};
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

void startGame() {
    clear();
    mvprintw(LINES / 2, COLS / 2 - 10, "Starting the game...");
    getch;
}
