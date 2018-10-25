#include "include/roundRobinWithPriority.h"
#include "drivers/include/console.h"
#include "include/comparators.h"
#include "include/malloc.h"
#include "include/scheduler.h"

void rrwp_enqueueThread(SCHEDULER_QUEUE* q, thread_t* t);
int rrwp_removeThread(SCHEDULER_QUEUE* q, thread_t* t);
int rrwp_calculateMaxTimeSlice(SCHEDULER_QUEUE* q, thread_t* t);
int rrwp_shouldCurrentBeEvicted(SCHEDULER_QUEUE* q);
thread_t* getNext(SCHEDULER_QUEUE* q);

void rrwp_enqueueThread(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;

    int currNice = getProcessByPID(t->process)->nice;

    if (currNice == 0) {
        scheduler_enqueue(t, q->nextQueue);
        return;
    }

    if (data->currentPriority == currNice - 1)
        linkedList_offer(data->backupQueue, t);
    else
        linkedList_offer(data->queues[currNice - 1], t);

    q->threadCount++;
}

int rrwp_removeThread(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;
    int currNice = getProcessByPID(t->process)->nice;

    int removed = linkedList_removeByValue(data->queues[currNice - 1], t, 1);
    if (removed == 0 && data->currentPriority == currNice - 1)
        removed += linkedList_removeByValue(data->backupQueue, t, 1);

    q->threadCount -= removed;

    return removed;
}

int calculateMaxTimeSlice(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;

    return getProcessByPID(t->process)->nice + 3;
}

int shouldCurrentBeEvicted(SCHEDULER_QUEUE* q) {
    data_RoundRobinWithPriority* data = q->queueData;
    thread_t* t = getCurrentThread();

    if (t->currTimeSlice % calculateMaxTimeSlice(q, t) == 0)
        return TRUE;
    else
        return FALSE;
}

thread_t* getNext(SCHEDULER_QUEUE* q) {
    // printf("ARRIBA");
    data_RoundRobinWithPriority* data = q->queueData;

    thread_t* t = NULL;
    linkedList_t aux;

    int currPriority = data->currentPriority;
    int basePriority = currPriority;
    if (linkedList_count(data->queues[currPriority]) == 0 &&
        linkedList_count(data->backupQueue) > 0) {
        aux = data->queues[currPriority];
        data->queues[currPriority] = data->backupQueue;
        data->backupQueue = aux;
        currPriority--;

        if (currPriority < 0) currPriority = MAX_PRIORITY - 1;
    }

    while (q->threadCount > 0 && t == NULL) {
        if (linkedList_count(data->queues[currPriority]) > 0) {
            data->currentPriority = currPriority;
            t = linkedList_poll(data->queues[currPriority]);
            t->currTimeSlice = 0;  // TODO: MOVER
            linkedList_offer(data->backupQueue, t);
        }

        currPriority--;
        if (currPriority < 0) currPriority = MAX_PRIORITY - 1;
    }

    // printf("ABAJO");

    return t;
}

thread_t* rrwp_queueSchedule(SCHEDULER_QUEUE* q, int force) {
    data_RoundRobinWithPriority* data = q->queueData;
    if (q->threadCount == 0) return NULL;

    thread_t* current = getCurrentThread();

    if (current->queueID == 0 && !shouldCurrentBeEvicted(q) && !force)
        return current;

    return getNext(q);
}

SCHEDULER_QUEUE* roundRobinWithPriority_newQueue(int queueQuantum,
                                                 int nextQueueID) {
    SCHEDULER_QUEUE* q = malloc(sizeof(SCHEDULER_QUEUE));
    data_RoundRobinWithPriority* data =
        malloc(sizeof(data_RoundRobinWithPriority));

    q->queueSchedule = rrwp_queueSchedule;
    q->removeThread = rrwp_removeThread;
    q->enqueueThread = rrwp_enqueueThread;

    q->queueQuantum = queueQuantum;
    q->threadCount = 0;
    q->nextQueue = nextQueueID;

    for (int i = 0; i < MAX_PRIORITY; i++) data->queues[i] = linkedList_new();

    data->backupQueue = linkedList_new();
    data->currentPriority = MAX_PRIORITY - 1;

    q->queueData = data;

    return q;
}