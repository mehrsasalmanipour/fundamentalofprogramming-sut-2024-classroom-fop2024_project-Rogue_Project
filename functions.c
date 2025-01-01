#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "functions.h"

#define MAX 100

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
    mvprintw(row, col, "Error: %s", message);
    refresh();
}

void getInputWithDisplay(int y, int x, char *input, int maxLen) {
    int ch;
    int i = 0;

    while (1) {
        ch = getch();
        if (ch == '\n' || ch == '\r') {
            if (i == 0) {
                mvprintw(y + 1, x, "Input cannot be empty!");
                refresh();
                getch();
                mvprintw(y + 1, x, "                      ");
                refresh();
                continue;
            }
            input[i] = '\0';
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvaddch(y, x + i, ' ');
                move(y, x + i);
                clrtoeol();
                refresh();
            }
        } else if (i < maxLen - 1 && isprint(ch)) {
            input[i++] = ch;
            mvaddch(y, x + i, ch);
        }
        refresh();
    }
}

void signUp() {
    char username[MAX], email[MAX], password[MAX];
    FILE *file = fopen("players.txt", "a");
    if (file == NULL) {
        displayError(LINES - 2, 0, "error opening file.");
        getch();
        return;
    }

    clear();
    mvprintw(5, 10, "Enter Username: ");
    refresh();
    getInputWithDisplay(5, 25, username, MAX);
    if (isUsernameTaken(username)) {
        displayError(7, 10, "Username already taken!");
        getch();
        fclose(file);
        return;
    }

    mvprintw(8, 10, "Enter Email: ");
    refresh();
    getInputWithDisplay(8, 25, email, MAX);
    if (!isValidEmail(email)) {
        displayError(10, 10, "Invalid email format!");
        getch();
        fclose(file);
        return;
    }

    mvprintw(11, 10, "Enter Password: ");
    refresh();
    getInputWithDisplay(11, 25, password, MAX);
    if (!isValidPassword(password)) {
        displayError(13, 10, "Password must be 7+ characters with a number, uppercase and lowercase");
        refresh();
        getch();
        fclose(file);
        return;
    }

    fprintf(file, "%s %s %s\n", username, email, password);
    fclose(file);

    mvprintw(15, 10, "Sign Up successful!");
    refresh();
    getch();
}
