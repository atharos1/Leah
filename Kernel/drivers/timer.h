#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H

typedef void (*function)(void);

void timer_Restart();
void timer_Tick();
void noTimer();
int timer_appendFunction(function f, unsigned long int ticks);
int timer_removeFunction(function f);

#endif
