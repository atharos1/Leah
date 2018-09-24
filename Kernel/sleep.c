#include "sleep.h"
#include <asm/libasm.h>
#include <scheduler.h>
#include "malloc.h"

#define TICKS_PER_SECOND 18.2

sleepingThreadNode * sleepingThreadList = NULL;

void sleepCurrentThread(int millis) {
    thread_t * thread = getCurrentThread();
    sleep(thread, millis);
    scheduler_dequeue_current();
    _force_scheduler();
}

void sleep(thread_t * thread, int millis) {

    thread->status = SLEEPING;

    sleepingThreadNode * n = malloc( sizeof(sleepingThreadNode) );

    n->thread = thread;
    int calMillis = (millis * TICKS_PER_SECOND) / 1000;
    //printf("calMillis: %d\n", calMillis);
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
            if(n == sleepingThreadList) //soy el primero
                sleepingThreadList = n->next;
            else
                prev->next = n->next;

            n->thread->status = READY;

            scheduler_enqueue(n->thread);

            free(n);

        }
        prev = n;
        n = n->next;
    }
}
