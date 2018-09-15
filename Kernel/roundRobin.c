#include "roundRobin.h"
#include "circularList.h"
#include "drivers/console.h"


thread_t * roundRobin_nextThread(SCHEDULER_QUEUE * q) {
    q->queue = next(q->queue);

    return getFirst(q->queue);
}

SCHEDULER_QUEUE * roundRobin_newQueue(int quantum) {
    SCHEDULER_QUEUE * q = getMemory( sizeof(SCHEDULER_QUEUE) );
    data_RoundRubin * data = getMemory( sizeof(data_RoundRubin) );

    q->queue = NULL;
    q->threadCount = 0;
    q->nextThread = roundRobin_nextThread;

    data->quantum = quantum;
    data->currQuantum = 0;

    q->queueData = data;

    return q;
}

