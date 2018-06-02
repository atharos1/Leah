#ifndef STD_IO
#define STD_IO

#define EOF -1

#define NUMCOLORS 16
static const enum COLOR {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE} COLOR;

int scanf(char * fmt, ...);
int sscanf(char* source, char* fmt, ...);
unsigned int getchar();
int printf(char * fmt, ...);
void printInt(int i);
unsigned int putchar(char c);
int puts(char * str);
void clearScreen();
void setBackgroundColor(enum COLOR backgroundColor);
void setFontColor(enum COLOR fontColor);



#endif