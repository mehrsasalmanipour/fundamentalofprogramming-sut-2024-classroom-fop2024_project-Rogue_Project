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

void signUp() {
    char username[MAX], email[MAX], password[MAX];
    FILE *file = fopen("players.txt", "a");
    if (file == NULL) {
        displayError(LINES - 2, 0, "Error opening file.");
        getch();
        return;
    }

    while (1) {
        clear();
        mvprintw(5, 10, "Enter Username (or type 'back' to return): ");
        refresh();
        getInputWithDisplay(5, 60, username, MAX);
        clrtoeol();
        if (strcmp(username, "back") == 0) {
            fclose(file);
            return;
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
            return;
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
            return;
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
}

void login() {
    char username[MAX], password[MAX];
    char fileUsername[MAX], fileEmail[MAX], filePassword[MAX];
    FILE *file = fopen("players.txt", "r");
    if (file == NULL) {
        displayError(5, 10, "Error opening file.");
        getch();
        return;
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
    } else {
        displayError(9, 10, "Invalid username or password.");
    }
    getch();
}
