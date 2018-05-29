#include <drivers/font.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_bPP 3

#define FONT_SIZE 2

void drawPixel(unsigned int x, unsigned int y, int color);
void drawChar(int x, int y, char character, int fontSize, int fontColor, int backgroundColor);
void invertPixel(unsigned int x, unsigned int y);
void invertChar(int x, int y);
void clearDisplay(unsigned int backgroundColor);
void drawSquare(unsigned int x, unsigned int y, int l, int color);
void drawRectangle(unsigned int x, unsigned int y, int b, int h, int color);