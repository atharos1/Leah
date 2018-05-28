#include <StandardLibrary/stdio.h>
#include <StandardLibrary/string.h>

#define CANTCOLORS 4

typedef void (*function)();
int _timerAppend(function f, unsigned long int ticks);
int _timerRemove(function f);
int _drawPixel(int x, int y, int color);

int color[CANTCOLORS] = {
    0xFFFFFF,
    0xFF0000,
    0x00FF00,
    0x0000FF
};

int currColor = 0;

void drawMe() {

    for(int i = 0; i < 200; i++)
        for(int j = 0; j < 200; j++)
            _drawPixel(i, j, color[currColor]);

    return;
}

void digitalClock() {

    char c;
    
    _timerAppend(drawMe, 60);

    while(c = getchar(), c != 27) { //Esc
        
        if(c == '\n') {
            if(currColor == CANTCOLORS)
                currColor = 0;
            else
                currColor++;
        }
        
    }

    _timerRemove(drawMe);

    return;

}