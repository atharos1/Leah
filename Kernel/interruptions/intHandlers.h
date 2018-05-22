#ifndef _intHandlers_
#define _intHandlers_

void timerTick();
int int80Handler(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx);

typedef void (*function)(void);

int appendFunctionToTimer(function f, unsigned long int ticks);
int removeFunctionFromTimer(function f);
void timerRestart();

#endif