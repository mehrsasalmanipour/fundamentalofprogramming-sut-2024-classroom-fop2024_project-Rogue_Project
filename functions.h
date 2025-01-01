#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void drawMenu(const char *menuItems[], int menuSize, int highlight);
int mainMenu();
bool isValidPassword(const char *password);
bool isValidEmail(const char *email);
bool isUsernameTaken(const char *username);
void displayError(int row, int col, const char *message);
void getInputWithDisplay(int y, int x, char *input, int maxLen);
void signUp();

#endif
