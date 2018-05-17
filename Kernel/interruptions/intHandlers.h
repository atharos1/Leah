#ifndef _intHandlers_
#define _intHandlers_

void timerTick();
void int80Handler(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx);

#endif