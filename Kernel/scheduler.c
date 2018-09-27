#include "include/scheduler.h"
#include "drivers/include/console.h"
#include "asm/libasm.h"
#include "include/lib.h"
#include "include/roundRobin.h"

#define TRUE 1
#define FALSE 0

SCHEDULER_QUEUE * Queues[MAX_QUEUE_COUNT];
int queueCount = 0;

int FORCE = FALSE;

struct currentThread {
    thread_t * thread;
    int queue;
} currentThread;


int pointer_cmp(void * p1, void * p2);
int is_thread_from_process_cmp(void * t, void * pid);
void * scheduler_nextTask(void * oldRSP);

void scheduler_init() {
    currentThread.thread = NULL;
    currentThread.queue = 0;

    Queues[queueCount++] = roundRobin_newQueue(2);

    purgeProcessList(FALSE); //Esto va acá?
}

void scheduler_enqueue(thread_t * thread) {
    if (thread == NULL)
      return;

    SCHEDULER_QUEUE * q = Queues[0];

    q->queue = insertAtEnd(q->queue, thread);
}

int scheduler_dequeue_thread(thread_t * t) {
    int status = 0;

    for(int currQueue = 0; currQueue < queueCount && status == 0; currQueue++)
        (Queues[currQueue])->queue = deleteByValue((Queues[currQueue])->queue, t, pointer_cmp, &status, 1);

    return status;
}

void scheduler_dequeue_process(int pid) {
    int status = 0;

    for(int currQueue = 0; currQueue < queueCount; currQueue++)
        (Queues[currQueue])->queue = deleteByValue((Queues[currQueue])->queue, &pid, is_thread_from_process_cmp, &status, 0);

    if(currentThread.thread->process == pid)
        FORCE = TRUE;
}

thread_t * scheduler_dequeue_current() {
    int status = 0;
    (Queues[currentThread.queue])->queue = deleteByValue((Queues[currentThread.queue])->queue, currentThread.thread, pointer_cmp, &status, 1);

    if(status != 0) {
        FORCE = TRUE;
        return currentThread.thread;
    } else
        return NULL;
}

int pointer_cmp(void * p1, void * p2) {
    return !(p1 == p2);
}

int is_thread_from_process_cmp(void * t, void * pid) {
    return ( ((thread_t*)t)->process == *((int*)pid) );
}

thread_t * getCurrentThread() {
    return currentThread.thread;
}

int getCurrentPID() {
    return currentThread.thread->process;
}

void * schedule(void * oldRSP) {
    if(FORCE == TRUE) {
        Queues[currentThread.queue]->restartEvictFunction(Queues[currentThread.queue]);
        FORCE = FALSE;
    }

    if( Queues[currentThread.queue]->checkEvictFunction(Queues[currentThread.queue]) )
        return scheduler_nextTask(oldRSP);
    else
        return oldRSP;
}

void * scheduler_nextTask(void * oldRSP) {
    if(currentThread.thread != NULL)
        currentThread.thread->stack.current = oldRSP;

    thread_t * nextThread = NULL;

    int currQueue;
    for(currQueue = 0; currQueue < queueCount; currQueue++) {
        nextThread = Queues[currQueue]->nextThreadFunction(Queues[currQueue]);
        if( nextThread != NULL ) {
            currentThread.thread = nextThread;
            currentThread.queue = currQueue;
            return nextThread->stack.current;
        }
    }

    currentThread.thread = getProcessByPID(0)->threadList[0];
    return currentThread.thread->stack.current; //Se ejecuta init
}
