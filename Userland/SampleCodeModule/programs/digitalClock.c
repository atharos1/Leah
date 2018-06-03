#include "../StandardLibrary/stdio.h"
#include "../StandardLibrary/string.h"

#define CANTCOLORS 5

typedef void (*function)();
int _timerAppend(function f, unsigned long int ticks);
int _timerRemove(function f);
int _drawPixel(int x, int y, int color);
int _setCursor(int x, int y);
int _setFontSize(unsigned int size);
int _rtc(int fetch);


int color[CANTCOLORS] = {
    0xFFFFFF,
    0xFF0000,
    0x00FF00,
    0x0000FF,
    0xFFA500
};

int currColor = 0;

void drawMe() {

    _setCursor(1, 1);
    printf("%X:%X:%X", _rtc(4), _rtc(2), _rtc(0));
    //printf("%d:%d:%d", 17, 25, 25);

    return;
}

void digitalClock() {

    char c;

    clearScreen();

    _setFontSize(1);
    _setCursor(38, 24);
    printf("Presione ENTER para cambiar el color del texto.");
    _setCursor(50, 25);
    printf("Presione ESC para salir.");

    _setFontSize(12);

    drawMe();

    _timerAppend(drawMe, 18);

    while(c = getchar(), c != 27) { //Esc

        if(c == '\n') {

            putchar('\7'); //BEEP

            if(currColor < CANTCOLORS - 1)
                currColor++;
            else
                currColor = 0;

            setFontColor(color[currColor]);
            drawMe();
        }

    }

    _timerRemove(drawMe);

    return;

}
