#include "../StandardLibrary/stdio.h"
#include "../StandardLibrary/string.h"
#include "../asm/asmLibC.h"

#define CANTCOLORS 5
#define CANTFREQ 4

typedef void (*function)();

int color[CANTCOLORS] = {
    0xFFFFFF,
    0xFF0000,
    0x00FF00,
    0x0000FF,
    0xFFA500
};

int frequence[CANTFREQ] = {440, 550, 660, 880};

int currFreq = 0;
int step = 1;
int currColor = 0;

void drawMe() {
    static int isDrawing = 0;

    if (!isDrawing) {
      isDrawing = 1;
      setCursor(1, 1);
      printf("%2X:%2X:%2X", sys_rtc(4), sys_rtc(2), sys_rtc(0));
      isDrawing = 0;
    }

    return;
}

void digitalClock() {

    char c;

    setBackgroundColor(0x000000);
    setFontColor(0xFFFFFF);

    clearScreen();

    setFontSize(1);
    setCursor(38, 24);
    printf("Presione ENTER para cambiar el color del texto.");
    setCursor(50, 25);
    printf("Presione ESC para salir.");

    setFontSize(12);

    drawMe();

    sys_timerAppend(drawMe, 18);

    while(c = getchar(), c != 27) { //Esc

        if(c == '\n') {

            if(currColor < CANTCOLORS - 1)
                currColor++;
            else
                currColor = 0;

            setFontColor(color[currColor]);
            sys_beep(frequence[currFreq], 4);

            currFreq += step;
            if (currFreq == 0 || currFreq == CANTFREQ - 1)
              step = -step;

            drawMe();
        }

    }

    sys_timerRemove(drawMe);

    return;

}
