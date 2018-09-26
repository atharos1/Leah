#include "timer.h"
#include "stdlib.h"
#include "../asm/asmLibC.h"

void * tickTimer(timer_t t) {
    while(1) {
        sys_sleep(t->millis);
        t->function();
    }
}

timer_t newTimer(void function(), int millis, int enabled) {
    timer_t t = malloc( sizeof(struct timer) );

    //t->enabled = enabled;
    t->millis = millis;
    t->function = function;
    t->thread = pthread_create(tickTimer, t);

    return t;
}

void cancelTimer(timer_t t) {
    if(t == NULL)
        return;

    pthread_cancel(t->thread);
    free(t);
}