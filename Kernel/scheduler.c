#include "scheduler.h"
#include "drivers/console.h"
#include "asm/libasm.h"
#include "lib.h"

#include "roundRobin.h"

#define TRUE 1
#define FALSE 0

SCHEDULER_QUEUE * Queues[MAX_QUEUE_COUNT];
int queueCount = 0;

int runningTasks;

struct currentThread {
    thread_t * thread;
    int queue;
} currentThread;

int threadCount = 0;
int FORCE = FALSE;

void scheduler_init() {

    currentThread.thread = NULL;
    currentThread.queue = 0;

    Queues[queueCount++] = roundRobin_newQueue(2);

    runningTasks = FALSE;
    threadCount = 0;

    purgeProcessList(); //Esto va acá?
    
}

int pointer_cmp(void * p1, void * p2) {
    return p1 == p2;
}

thread_t * scheduler_dequeue_current() {
    int status = 0;
    (Queues[currentThread.queue])->queue = deleteByValue((Queues[currentThread.queue])->queue, currentThread.thread, pointer_cmp, &status);

    Queues[currentThread.queue]->threadCount--;
    threadCount--;
    FORCE = TRUE;
    return currentThread.thread;
}

/*void scheduler_dequeue_process(int pid) {
    //printf("hola");
    return;
}*/

void scheduler_enqueue(thread_t * thread) {

    SCHEDULER_QUEUE * q = Queues[0];

    q->queue = insertAtEnd(q->queue, thread);
    q->threadCount++;
    threadCount++;

}

void * scheduler_nextTask(void * oldRSP) {

    if( threadCount == 0 ) {
        return oldRSP;
    }
       
    FORCE = FALSE;
    if(runningTasks == TRUE) {
        getCurrentThread()->stack.current = oldRSP;
    }
    runningTasks = TRUE;

    thread_t * nextThread = NULL;    
    
    
    
    int currQueue;

    for(currQueue = 0; currQueue < queueCount; currQueue++) {
        nextThread = Queues[currQueue]->nextThreadFunction(Queues[currQueue], oldRSP);
        if( nextThread != NULL ) {
            //printf("Sale: %s | Entra: %s\n", getProcessByPID(getCurrentThread()->process)->name, getProcessByPID(nextThread->process)->name);
            currentThread.thread = nextThread;
            currentThread.queue = currQueue;
            FORCE = FALSE;

            return nextThread->stack.current;
        }
    }

    return oldRSP;

}


void * schedule(void * oldRSP) {

    if( Queues[currentThread.queue]->checkEvictFunction(Queues[currentThread.queue]) || FORCE == TRUE )
        return scheduler_nextTask(oldRSP);
    else
        return oldRSP;

}

thread_t * getCurrentThread() {
    return currentThread.thread;
}

int getCurrentPID() {
    return currentThread.thread->process;
}

/*void scheduler_dequeue(thread_t * thread) {

    int deleted = FALSE;

    for(int i = 0; (i < queueCount && deleted == FALSE); i++)
        Queues[i]->queue = deleteByValue(Queues[i]->queue, thread, pointer_cmp, &deleted);

    /*NODE * auxQueue = mainQueue;
    SCHEDULER_QUEUE * q;
    
    do {
        q = getFirst(auxQueue);
        q->queue = deleteByValue(q->queue, thread, pointer_cmp, &deletionStatus);
        if(deletionStatus == 1) {
            return;
        }

        auxQueue = next(auxQueue);
    } while( auxQueue != mainQueue );*/

    /*return;
}*/