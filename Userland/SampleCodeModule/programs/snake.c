#include "../StandardLibrary/stdio.h"
#include "../StandardLibrary/string.h"
#include <stdint.h>
#include "../asm/asmLibC.h"
#include "../StandardLibrary/timer.h"

#define NULL ((void *) 0)

#define X 0
#define Y 1
#define SQUARE_SIZE 32
#define SNAKE_MAX_LENGHT 500

int snake[SNAKE_MAX_LENGHT][2] = {{0}};

int screen_width = 1024 / SQUARE_SIZE;
int screen_height = 768 / SQUARE_SIZE;
int grow_rate = 3;

int comidita[2] = {3, 3};

typedef enum direction { ARRIBA, ABAJO, IZQUIERDA, DERECHA } direction;

enum direction dir = ARRIBA;
int snakeLength;
int startLenght;
int status = 0;

unsigned short lfsr = 0xACE1u;
unsigned bit;

int game_start(int ticks, int growrate);

unsigned rand() {
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

void drawRectangle(unsigned int x, unsigned int y, int b, int h, int color) {
    for(int i = 0; i < b; i++)
		for(int j = 0; j < h; j++)
			sys_drawPixel(x + i, y + j, color);
}

void drawSquare(unsigned int x, unsigned int y, int l, int color) {
    drawRectangle(x, y, l, l, color);
}


void drawComidita() {
    comidita[X] = rand() % screen_width;
    comidita[Y] = rand() % screen_height;

    for (int i = 0; i < snakeLength; i++) {
        if (snake[i][X] == comidita[X] && snake[i][Y] == comidita[Y]) {
            comidita[X] = rand() % screen_width;
            comidita[Y] = rand() % screen_height;
            i = -1;
        }
    }

    drawSquare(comidita[X]*SQUARE_SIZE, comidita[Y]*SQUARE_SIZE, SQUARE_SIZE, 0x00FF00);
}

void * move(void * args) {
    int c;
    //int k = 0;
    /*while((c = getchar()) != 27) {
      k = c;
      if (c == 27) {
          status = -1;
          return;
      }
    }*/

    while((c = getchar()) != 27) {
        switch (c) {
            case 'w':
                if (dir != ABAJO)
                dir = ARRIBA;
                break;
            case 's':
                if (dir != ARRIBA)
                dir = ABAJO;
                break;
            case 'd':
                if (dir != IZQUIERDA)
                dir = DERECHA;
                break;
            case 'a':
                if (dir != DERECHA)
                dir = IZQUIERDA;
                break;
        }
    }

    status = -1;
    return 0;

}

void refresh() {

    //move();

    //Borro la cola
    if( snake[0][X] == comidita[X] && snake[0][Y] == comidita[Y] ) {
        snakeLength+= grow_rate;

        if(snakeLength >= SNAKE_MAX_LENGHT) {
            status = 2;
        }

        printf("\7");
        drawComidita();
    } else {
        drawSquare(snake[snakeLength-1][X]*SQUARE_SIZE, snake[snakeLength-1][Y]*SQUARE_SIZE, SQUARE_SIZE, 0x000000);
    }

    for (int i = snakeLength - 1;i > 0; i--) {
        if (snake[i][X] == snake[0][X] && snake[i][Y] == snake[0][Y]) {
            status = 1;
        }

        snake[i][X] = snake[i - 1][X];
        snake[i][Y] = snake[i - 1][Y];
    }


    //printf("(%d, %d)", snake[0][X], snake[0][Y]);

    switch (dir) {
        case ARRIBA:
              snake[0][Y] = (snake[0][Y] - 1);
            break;
        case ABAJO:
              snake[0][Y] = (snake[0][Y] + 1);
            break;
        case DERECHA:
              snake[0][X] = (snake[0][X] + 1);
            break;
        case IZQUIERDA:
              snake[0][X] = (snake[0][X] - 1);
            break;
    }

    if(snake[0][X] == screen_width)
        snake[0][X] = 0;

    if(snake[0][Y] == screen_height)
        snake[0][Y] = 0;

    if(snake[0][X] == -1)
        snake[0][X] = screen_width - 1;

    if(snake[0][Y] == -1)
        snake[0][Y] = screen_height - 1;

    //Dibujo la cabeza
    drawSquare(snake[0][X]*SQUARE_SIZE, snake[0][Y]*SQUARE_SIZE, SQUARE_SIZE, 0xFFFFFF);

    /*if (snake[0][X] == comidita[X] && snake[0][Y] == comidita[Y]) {
        snakeLength++;
        nuevaComidita();
    }*/
}

void displayInstructions() {
    setFontSize(5);
    //printf("MY LITTLE BOA CONSTRICTOR\n");

    char title[26] = "MY LITTLE BOA CONSTRICTOR";

    int colors[7] = {
        0x4444DD,
        0x11aabb,
        0xaacc22,
        0xd0c310,
        0xff9933,
        0xff4422,
        0x72a4c9
    };

    int currColor = 0;
    for(int i = 0; i < 26; i++) {
        if(title[i] != ' ') {
            if(currColor == 6)
                currColor = 0;
            else
                currColor++;

            setFontColor( colors[currColor] );
        }
        putchar( title[i] );
    }

    setFontSize(2);
    printf("\nQue pony ni que pony?");

    setFontColor(0xFFFFFFFF);

    setFontSize(3);
    printf("\nInstrucciones");
    setFontSize(2);
    printf("\n\n\nTeclas de movimiento:\nW (Arriba)\nA (Izquierda)\nS (Abajo)\nD (Derecha)");

    printf("\n\nDurante el juego, pulse ESC para salir");
    printf("\nPRESIONE ENTER PARA COMENZAR\n\n\n");

    int c;
    while( (c = getchar()) != '\n' && c != 27);

    if (c == 27)
        status = -1;

    clearScreen();

}

int snake_main(char ** args) {

    int ticks = 2;
    int growrate = 4;

    if(args[0] != NULL) {
        ticks = atoi(args[0]);

        if(args[1] != NULL)
            growrate = atoi(args[1]);
    }

    game_start(ticks, growrate);
    return 0;

}

int game_start(int ticks, int growrate) {
    grow_rate = growrate;
    setBackgroundColor(0x000000);
    setFontColor(0xFFFFFF);
    clearScreen();
    status = 0;
    displayInstructions();

    for(int i = 0; i < 500; i ++) {
        snake[i][X] = snake[i][Y] = -1;
    }

    dir = ARRIBA;

    int mid_x = screen_width / 2;
    int mid_y = screen_height / 2;

    snake[0][X] = mid_x;
    snake[0][Y] = mid_y;
    drawSquare(snake[0][X]*SQUARE_SIZE, snake[0][Y]*SQUARE_SIZE, SQUARE_SIZE, 0xFFFFFF);

    snake[1][X] = mid_x;
    snake[1][Y] = mid_y + 1;
    drawSquare(snake[1][X]*SQUARE_SIZE, snake[1][Y]*SQUARE_SIZE, SQUARE_SIZE, 0xFFFFFF);

    snake[2][X] = mid_x;
    snake[2][Y] = mid_y + 2;
    drawSquare(snake[2][X]*SQUARE_SIZE, snake[2][Y]*SQUARE_SIZE, SQUARE_SIZE, 0xFFFFFF);

    snake[3][X] = mid_x;
    snake[3][Y] = mid_y + 3;
    drawSquare(snake[3][X]*SQUARE_SIZE, snake[3][Y]*SQUARE_SIZE, SQUARE_SIZE, 0xFFFFFF);

    startLenght = snakeLength = 4;

    drawComidita();

    //sys_timerAppend(refresh, ticks);

    pthread_t moveListener = pthread_create(move, NULL);
    timer_t refreshTimer = newTimer(refresh, 1, TRUE);

    while(status == 0) {
        //status = checkStatus();
    }

    //sys_timerRemove(refresh);
    cancelTimer(refreshTimer);
    pthread_cancel(moveListener);

    if(status == -1) //Salio
        return -1;
    else
        return (snakeLength - startLenght) / grow_rate;

    return status; //Perdiste, perdiste, no hay nadie peor que vos...

}
