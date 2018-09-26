#ifndef _PRODCONS_H
#define _PRODCONS_H

#define QUIT 'q'
#define INC_CHEF 'c'
#define DEC_CHEF 'o'
#define INC_WAITER 'm'
#define DEC_WAITER 'e'
#define MAX_PRODCONS 10
#define TRAYS_QTY 20
#define EMPTY_SPACE '-'

int prodcons();
void printMenu();
void printTrays();
void callChef();
void * chef();
void restChef();
void chefSuicide();
void callWaiter();
void * waiter();
void restWaiter();
void waiterSuicide();
void terminateAll();

#endif
