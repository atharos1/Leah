#ifndef _PHILOS_H
#define _PHILOS_H

#define QUIT 'q'
#define INC 'b'
#define DEC 'd'
#define MAX_PHI 20

int philosophers();
void printMenu();
void printTable();
void born();
void * philosopher(void * args);
void die();
void philosopherSuicide();
void terminateAll();

#endif
