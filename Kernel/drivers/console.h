#ifndef CONSOLE_H
#define CONSOLE_H

#define TAB_WIDTH 4

//Librería screen
void setCursor(unsigned short int x, unsigned short int y);
void moveCursor(int cant);
void setFontColor(int color);
void setBackgroundColor(int color);
void setFontSize(unsigned int size);
int getFontColor();
int getBackgroundColor();
int getFontSize();
void setGraphicCursorStatus(unsigned int status);
void printf(char * format, ...);
void printChar(char c);
void printInt(int i);
void printBase(int i, int base);
void printString(char * str);
void incLine(int cant);
void clearScreen();


#endif
