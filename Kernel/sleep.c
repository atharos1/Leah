#include "sleep.h"
#include <asm/libasm.h>

#define TICKS_PER_SECOND 18.2

sleepingThreadNode * sleepingThreadList = NULL;

void sleepCurrentThread(int millis) {
    thread_t * thread = scheduler_dequeue_current();
    sleep(thread, millis);
    _force_timer_tick();
}

void sleep(thread_t * thread, int millis) {

    thread->status = SLEEPING;

    sleepingThreadNode * n = getMemory( sizeof(sleepingThreadNode) );

    n->thread = thread;
    int calMillis = (millis / 1000) * TICKS_PER_SECOND;
    n->ticks_remaining = (calMillis > 0 ? calMillis : 1);

    n->next = sleepingThreadList;
    sleepingThreadList = n;

}

void sleep_update() {
    sleepingThreadNode * n = sleepingThreadList;
    sleepingThreadNode * prev = NULL;

    while(n != NULL) {
        n->ticks_remaining--;
        if(n->ticks_remaining == 0) {
            if(prev == NULL) //soy el primero
                sleepingThreadList = n->next;
            else
                prev->next = n->next;

            n->thread->status = READY;

            scheduler_enqueue(n->thread);

            freeMemory(n);

        }
        prev = n;
        n = n->next;
    }
}