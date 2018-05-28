#ifndef CONSOLE_H
#define CONSOLE_H

static const enum COLOR {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE} COLOR;

//Librería screen
void setCursor(unsigned short int x, unsigned short int y);
void moveCursor(int cant);
void setFontColor(int color);
void setBackgroundColor(int color);
void writeStringToScreen(int x, int y, char* string);
void printf(char * format, ...);
void printChar(char c);
void printInt(int i);
void printBase(int i, int base);
void printString(char * str);
void incLine(int cant);
void clearScreen();
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
void cursorTick(); //No tendría que estar acá, pero bue...

//Librería screen


//Librería math
int sign(int n);
int abs(int n);
//Librería math

#endif
