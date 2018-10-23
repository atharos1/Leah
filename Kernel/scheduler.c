#include "include/scheduler.h"
#include "asm/libasm.h"
#include "drivers/include/console.h"
#include "drivers/include/kb_driver.h"
#include "drivers/include/timer.h"
#include "include/comparators.h"
#include "include/lib.h"
#include "include/roundRobin.h"
#include "include/roundRobinWithPriority.h"
#include "interruptions/defs.h"

#define TRUE 1
#define FALSE 0

SCHEDULER_QUEUE *Queues[MAX_QUEUE_COUNT];
int queueCount = 0;

int FORCE = FALSE;

thread_t *currentThread;

void *scheduler_nextTask(void *oldRSP);

void scheduler_init() {
    currentThread = NULL;

    Queues[1] = roundRobin_newQueue(4);
    Queues[0] = roundRobinWithPriority_newQueue(3, Queues[0]);
    queueCount = 2;

    purgeProcessList(FALSE);  // Esto va acá?

    /*timer_appendFunction(scheduler_midTerm,
                         PIT_FREQUENCY * 10);  // Cada 10 segundos*/
}

void scheduler_midTerm() {
    for (int currQueue = 0; currQueue < queueCount; currQueue++)
        if (Queues[currQueue]->ageThreads != NULL)
            Queues[currQueue]->ageThreads(Queues[currQueue]);
}

void scheduler_enqueue(thread_t *thread, int queue) {
    if (thread == NULL) return;

    thread->queueID = queue;

    SCHEDULER_QUEUE *q = Queues[queue];

    q->addToQueue(q, thread);
    thread->queue = q;
}

int scheduler_dequeue_thread(thread_t *t) {
    if (t == currentThread) return (t == scheduler_dequeue_current());

    return ((SCHEDULER_QUEUE *)(t->queue))->removeThread(t->queue, t);
}

void scheduler_dequeue_process(int pid) {
    process_t *p = getProcessByPID(pid);

    for (int i = 0; i < MAX_THREAD_COUNT; i++)
        if (p->threadList[i] != NULL)
            scheduler_dequeue_thread(p->threadList[i]);
}

thread_t *scheduler_dequeue_current() {
    if (((SCHEDULER_QUEUE *)(currentThread->queue))
            ->removeThread(currentThread->queue, currentThread) >= 1) {
        FORCE = TRUE;
        return currentThread;
    } else {
        return NULL;
    }
}

thread_t *getCurrentThread() { return currentThread; }

int getCurrentPID() { return currentThread->process; }

void *scheduler_shortTerm(void *oldRSP) {
    if (FORCE == TRUE) {
        if (currentThread != NULL)
            ((SCHEDULER_QUEUE *)(currentThread->queue))
                ->restartEvictFunction(currentThread->queue);
        FORCE = FALSE;
    }

    if (aliveProcessCount() > 0 &&
        (currentThread == NULL ||
         ((SCHEDULER_QUEUE *)(currentThread->queue))
             ->checkEvictFunction(currentThread->queue)))
        return scheduler_nextTask(oldRSP);
    else
        return oldRSP;
}

void *scheduler_nextTask(void *oldRSP) {
    if (currentThread != NULL) currentThread->stack.current = oldRSP;

    thread_t *nextThread = NULL;

    int currQueue;
    for (currQueue = 0; currQueue < queueCount; currQueue++) {
        nextThread = Queues[currQueue]->nextThreadFunction(Queues[currQueue]);

        if (nextThread != NULL) {
            currentThread = nextThread;

            if (aliveProcessCount() <= 2) giveForeground(nextThread->process);

            return nextThread->stack.current;
        }
    }

    currentThread = getProcessByPID(0)->threadList[0];
    return currentThread->stack.current;  // Se ejecuta init
}
