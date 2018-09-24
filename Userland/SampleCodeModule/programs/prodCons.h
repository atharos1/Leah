#ifndef _PRODCONS_H
#define _PRODCONS_H

#define QUIT 'q'
#define INC_CHEF 'c'
#define DEC_CHEF 'v'
#define INC_WAITER 'w'
#define DEC_WAITER 'e'
#define MAX_PRODCONS 10
#define TRAYS_QTY 20
#define EMPTY_SPACE '-'

void prodcons();
void printMenu();
void initTrays(char * trays);
void printTrays(char * trays);
void callChef();
void * chef();
void restChef();
void callWaiter();
void * waiter();
void restWaiter();
void terminateAll();

#endif
