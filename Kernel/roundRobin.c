#include "roundRobin.h"
#include "circularList.h"
#include "drivers/console.h"
#include "malloc.h"

thread_t * roundRobin_nextThread(SCHEDULER_QUEUE * q) {
    data_RoundRubin * data = q->queueData;
    data->currQuantum = 0;

    thread_t * ret = getFirst(q->queue);
    q->queue = next(q->queue);

    return ret;
}

int roundRobin_checkEvict(SCHEDULER_QUEUE * q) {

    data_RoundRubin * data = q->queueData;

    data->currQuantum++;

    if(data->currQuantum % data->quantum == 0) {
        data->currQuantum = 0;
        return TRUE;
    }

    return FALSE;
}

void roundRobin_restartEvict(SCHEDULER_QUEUE * q) {
    data_RoundRubin * data = q->queueData;
    data->currQuantum = data->quantum - 1; //Garantizo que checkEvict retorne true en la próxima ejecución
}

SCHEDULER_QUEUE * roundRobin_newQueue(int quantum) {
    SCHEDULER_QUEUE * q = malloc( sizeof(SCHEDULER_QUEUE) );
    data_RoundRubin * data = malloc( sizeof(data_RoundRubin) );

    q->queue = NULL;
    //q->threadCount = 0;
    q->nextThreadFunction   = roundRobin_nextThread;
    q->checkEvictFunction   = roundRobin_checkEvict;
    q->restartEvictFunction = roundRobin_restartEvict;

    data->quantum = quantum;
    data->currQuantum = 0;

    q->queueData = data;

    return q;
}
