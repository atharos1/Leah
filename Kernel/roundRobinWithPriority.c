#include "include/roundRobinWithPriority.h"
#include "include/circularList.h"
#include "drivers/include/console.h"
#include "include/malloc.h"

thread_t * roundRobinWithPriority_nextThread(SCHEDULER_QUEUE * q) {

    data_RoundRobinWithPriority * data = q->queueData;
    if(data->currentMaxPriority == -1)
        return NULL;

    data->currQuantum = 0;

    thread_t * ret = getFirst(data->priorityArray[data->currentMaxPriority]);

    /*if(data->currentMaxPriority == 0) {
        if(data->nextQueue != NULL) {
            //Mover a la proxima cola
        }
            //
    }*/



    q->queue = next(q->queue);

    return ret;
}

int roundRobinWithPriority_checkEvict(SCHEDULER_QUEUE * q) {

    data_RoundRobinWithPriority * data = q->queueData;

    data->currQuantum++;

    if(data->currQuantum % data->quantum == 0) {
        data->currQuantum = 0;
        return TRUE;
    }

    return FALSE;
}

void roundRobinWithPriority_restartEvict(SCHEDULER_QUEUE * q) {
    data_RoundRobinWithPriority * data = q->queueData;
    data->currQuantum = data->quantum - 1; //Garantizo que checkEvict retorne true en la próxima ejecución
}

SCHEDULER_QUEUE * roundRobinWithPriority_newQueue(int quantum) {
    SCHEDULER_QUEUE * q = malloc( sizeof(SCHEDULER_QUEUE) );
    data_RoundRobinWithPriority * data = malloc( sizeof(data_RoundRobinWithPriority) );

    q->queue = NULL;
    //q->threadCount = 0;
    q->nextThreadFunction   = roundRobinWithPriority_nextThread;
    q->checkEvictFunction   = roundRobinWithPriority_checkEvict;
    q->restartEvictFunction = roundRobinWithPriority_restartEvict;

    data->quantum = quantum;
    data->currQuantum = 0;

    q->queueData = data;

    return q;
}
