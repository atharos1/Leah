#ifndef _PHILOS_H
#define _PHILOS_H

#define QUIT 'q'
#define INC 'b'
#define DEC 'd'
#define MAX_PHI 20

int philosophers();
void born();
void * philosopher(void * args);
void die();
void philosopherSuicide(int id);

#endif
