#ifndef STD_IO
#define STD_IO

#define EOF -1

#define NUMCOLORS 16

#include <stdint.h>

static const enum COLOR {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE} COLOR;

unsigned int getchar();
int printf(char * fmt, ...);
void printInt(int i);
unsigned int putchar(char c);
int puts(char * str);
void clearScreen();
void setBackgroundColor(unsigned int color);
void setFontColor(unsigned int color);
int setFontSize(unsigned int size);
unsigned int getBackgroundColor();
unsigned int getFontColor();
int getFontSize();
int sscanf(char* source, char* format, ...);
void setGraphicCursorStatus(unsigned int status);
int setCursor(unsigned int x, unsigned int y);
char * itoa(uint64_t value, char * buffer, uint32_t base);


#endif
