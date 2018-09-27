#ifndef STD_TIMER
#define STD_TIMER

#define TRUE 1
#define FALSE 0

#include "pthread.h"
#include "sem.h"

struct timer {
    int semEnabled;
    pthread_t thread;
    void (*function)();
    int millis;
};

typedef struct timer * timer_t;
timer_t newTimer(void function(), int millis, int enabled);
void cancelTimer(timer_t t);

#endif