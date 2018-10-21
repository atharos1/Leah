#include "include/sleep.h"
#include "asm/libasm.h"
#include "include/scheduler.h"
#include "include/malloc.h"
#include "interruptions/defs.h"

sleepingThreadNode *sleepingThreadList = NULL;

void sleepCurrentThread(int millis)
{
    thread_t *thread = getCurrentThread();
    sleep(thread, millis);
    scheduler_dequeue_current();
    _force_scheduler();
}

void sleep(thread_t *thread, int millis)
{

    thread->status = SLEEPING;

    sleepingThreadNode *n = malloc(sizeof(sleepingThreadNode));

    n->thread = thread;
    int calMillis = (millis * PIT_FREQUENCY) / 1000;
    //printf("calMillis: %d\n", calMillis);
    n->ticks_remaining = (calMillis > 0 ? calMillis : 1);

    n->next = sleepingThreadList;
    sleepingThreadList = n;
}

void sleep_update()
{
    sleepingThreadNode *n = sleepingThreadList;
    sleepingThreadNode *prev = NULL;
    sleepingThreadNode *aux;

    while (n != NULL)
    {
        n->ticks_remaining--;
        if (n->ticks_remaining == 0)
        {
            if (n == sleepingThreadList) //soy el primero
                sleepingThreadList = n->next;
            else
                prev->next = n->next;

            if (n->thread->status == DEAD)
            {
                eraseTCB(n->thread);
            }
            else
            {
                n->thread->status = READY;
                scheduler_enqueue(n->thread);
            }

            aux = n;
            n = n->next;
            free(aux);
        }
        else
        {
            prev = n;
            n = n->next;
        }
    }
}
