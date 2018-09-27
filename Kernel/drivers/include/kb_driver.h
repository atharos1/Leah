#ifndef KB_DRIVER_H
#define KB_DRIVER_H

#define EOF -1

#include "../../include/process.h"

void init_kb();
char kb_fetch();
char getChar();
void giveForeground(int pid);
int getForegroundPID();
//unsigned char peekChar();
//void ungetc(unsigned char c);

#endif
