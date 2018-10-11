#include "include/roundRobin.h"
#include "drivers/include/console.h"
#include "include/circularList.h"
#include "include/comparators.h"
#include "include/malloc.h"

thread_t* roundRobin_nextThread(SCHEDULER_QUEUE* q) {
    data_RoundRobin* data = q->queueData;
    data->currQuantum = 0;

    thread_t* ret = getFirst(q->queue);
    q->queue = next(q->queue);

    return ret;
}

int roundRobin_removeThread(SCHEDULER_QUEUE* q, thread_t* t) {
    int status = 0;

    q->queue = deleteByValue(q->queue, t, pointer_cmp, &status, 1);

    return status;
}

void roundRobin_addToQueue(SCHEDULER_QUEUE* q, thread_t* t) {
    q->queue = insertAtEnd(q->queue, t);
}

int roundRobin_checkEvict(SCHEDULER_QUEUE* q) {
    data_RoundRobin* data = q->queueData;

    data->currQuantum++;

    if (data->currQuantum % data->quantum == 0) {
        data->currQuantum = 0;
        return TRUE;
    }

    return FALSE;
}

void roundRobin_restartEvict(SCHEDULER_QUEUE* q) {
    data_RoundRobin* data = q->queueData;
    data->currQuantum =
        data->quantum -
        1;  // Garantizo que checkEvict retorne true en la próxima ejecución
}

SCHEDULER_QUEUE* roundRobin_newQueue(int quantum) {
    SCHEDULER_QUEUE* q = malloc(sizeof(SCHEDULER_QUEUE));
    data_RoundRobin* data = malloc(sizeof(data_RoundRobin));

    q->queue = NULL;
    q->nextThreadFunction = roundRobin_nextThread;
    q->checkEvictFunction = roundRobin_checkEvict;
    q->restartEvictFunction = roundRobin_restartEvict;
    q->addToQueue = roundRobin_addToQueue;
    q->removeThread = roundRobin_removeThread;
    q->ageThreads = NULL;

    data->quantum = quantum;
    data->currQuantum = 0;

    q->queueData = data;

    return q;
}
