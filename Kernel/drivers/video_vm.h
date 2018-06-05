#include <drivers/font.h>

extern unsigned int SCREEN_WIDTH;
extern unsigned int SCREEN_HEIGHT;
extern unsigned int SCREEN_bPP;

void init_VM_Driver();
void drawPixel(unsigned int x, unsigned int y, int color);
void drawChar(int x, int y, char character, int fontSize, int fontColor, int backgroundColor);
void invertPixel(unsigned int x, unsigned int y);
void invertChar(int x, int y);
void clearDisplay(unsigned int backgroundColor);
void drawSquare(unsigned int x, unsigned int y, int l, int color);
void drawRectangle(unsigned int x, unsigned int y, int b, int h, int color);
void scrollUp(int cant, unsigned int backgroundColor);
