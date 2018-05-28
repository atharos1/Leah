#include <drivers/font.h>

void drawPixel(unsigned int x, unsigned int y, int color);
void drawChar(unsigned int x, unsigned int y, char c, int fontColor, int backgroundColor);
void invertPixel(unsigned int x, unsigned int y);
void invertChar(int x, int y);
void clearDisplay();