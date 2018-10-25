#include "include/roundRobin.h"
#include "drivers/include/console.h"
#include "include/circularList.h"
#include "include/comparators.h"
#include "include/malloc.h"

void enqueueThread(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobin* data = q->queueData;
    data->queue = insertAtEnd(data->queue, t);
    q->threadCount++;
}

int removeThread(SCHEDULER_QUEUE* q, thread_t* t) {
    int removed = 0;

    data_RoundRobin* data = q->queueData;

    data->queue = deleteByValue(data->queue, t, pointer_cmp, &removed, 1);

    q->threadCount -= removed;

    return removed;
}

thread_t* queueSchedule(SCHEDULER_QUEUE* q, int force) {
    data_RoundRobin* data = q->queueData;

    thread_t* ret = getFirst(data->queue);
    data->queue = next(data->queue);

    return ret;
}

SCHEDULER_QUEUE* roundRobin_newQueue(int queueQuantum, int nextQueueID) {
    SCHEDULER_QUEUE* q = malloc(sizeof(SCHEDULER_QUEUE));
    data_RoundRobin* data = malloc(sizeof(data_RoundRobin));

    q->queueSchedule = queueSchedule;
    q->removeThread = removeThread;
    q->enqueueThread = enqueueThread;

    q->queueQuantum = queueQuantum;
    q->threadCount = 0;
    q->nextQueue = nextQueueID;

    data->queue = NULL;

    q->queueData = data;

    return q;
}
