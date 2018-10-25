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
int currQueue;
int currTimeSlice = 0;

int FORCE = FALSE;

thread_t *currentThread;

void resetCurrentThread() { currentThread = NULL; }

thread_t *getCurrentThread() { return currentThread; }

void *scheduler_nextTask(void *oldRSP);

void scheduler_init() {
    currentThread = NULL;

    Queues[1] = roundRobin_newQueue(3, -1);
    Queues[0] = roundRobinWithPriority_newQueue(8, 1);
    queueCount = 2;
    currQueue = 0;

    purgeProcessList(FALSE);  // Esto va acá?

    /*timer_appendFunction(scheduler_midTerm,
                         PIT_FREQUENCY * 10);  // Cada 10 segundos*/
}

/*void scheduler_midTerm() {
    for (int currQueue = 0; currQueue < queueCount; currQueue++)
        if (Queues[currQueue]->ageThreads != NULL)
            Queues[currQueue]->ageThreads(Queues[currQueue]);
}*/

void scheduler_enqueue(thread_t *thread, int queue) {
    if (thread == NULL) return;

    thread->queueID = queue;
    SCHEDULER_QUEUE *q = Queues[queue];
    q->enqueueThread(q, thread);
}

int scheduler_dequeue_thread(thread_t *t) {
    if (t == currentThread) return (t == scheduler_dequeue_current());

    return Queues[t->queueID]->removeThread(Queues[t->queueID], t);
}

void scheduler_dequeue_process(int pid) {
    process_t *p = getProcessByPID(pid);

    for (int i = 0; i < MAX_THREAD_COUNT; i++)
        if (p->threadList[i] != NULL) {
            scheduler_dequeue_thread(p->threadList[i]);
        }
}

thread_t *scheduler_dequeue_current() {
    if (Queues[currentThread->queueID]->removeThread(
            Queues[currentThread->queueID], currentThread) >= 1) {
        thread_t *ret = currentThread;
        FORCE = TRUE;
        // resetCurrentThread();
        return ret;
    } else {
        return NULL;
    }
}

int getCurrentPID() { return currentThread->process; }

void *scheduler_shortTerm(void *oldRSP) {
    thread_t *t = NULL;

    if (aliveProcessCount() == 0) return oldRSP;

    int forced = FORCE;

    if (currTimeSlice > Queues[currQueue]->queueQuantum) {
        if (currQueue == queueCount - 1)
            currQueue = 0;
        else
            currQueue++;
        currTimeSlice = 0;
    }

    if (!forced) currTimeSlice++;

    if (currentThread != NULL) {
        currentThread->stack.current = oldRSP;
    }

    if (Queues[currQueue]->queueQuantum)

        for (int i = 0; i < queueCount; i++) {
            t = Queues[currQueue]->queueSchedule(Queues[currQueue], forced);

            if (t != NULL) break;

            if (currQueue == queueCount - 1)
                currQueue = 0;
            else
                currQueue++;
        }

    if (t != NULL) {
        currentThread = t;
        if (aliveProcessCount() <= 2) giveForeground(t->process);
    } else {
        if (currentThread == NULL)
            getProcessByPID(0)->threadList[0]->stack.current = oldRSP;
        currentThread = getProcessByPID(0)->threadList[0];
    }

    FORCE = FALSE;

    return currentThread->stack.current;
}