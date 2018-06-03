#include "../StandardLibrary/stdio.h"
#include "../StandardLibrary/string.h"

#define CANTCOLORS 5
#define CANTFREQ 8

typedef void (*function)();
int _timerAppend(function f, unsigned long int ticks);
int _timerRemove(function f);
int _drawPixel(int x, int y, int color);
int _setCursor(int x, int y);
int _setFontSize(unsigned int size);
int _rtc(int fetch);
void _beep(int nFrequence, unsigned char duration);


int color[CANTCOLORS] = {
    0xFFFFFF,
    0xFF0000,
    0x00FF00,
    0x0000FF,
    0xFFA500
};

int frequence[] = {440, 495, 550, 587, 660, 733, 825, 880};

int currFreq = 0;
int step = 1;
int currColor = 0;

void drawMe() {
    static int isDrawing = 0;

    if (!isDrawing) {
      isDrawing = 1;
      _setCursor(1, 1);
      printf("%2X:%2X:%2X", _rtc(4), _rtc(2), _rtc(0));
      //printf("%d:%d:%d", 17, 25, 25);
      isDrawing = 0;
    }

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

            //putchar('\7'); //BEEP

            if(currColor < CANTCOLORS - 1)
                currColor++;
            else
                currColor = 0;

            setFontColor(color[currColor]);
            _beep(frequence[currFreq], 4);

            currFreq += step;
            if (currFreq == 0 || currFreq == CANTFREQ - 1)
              step = -step;

            drawMe();
        }

    }

    _timerRemove(drawMe);

    return;

}
