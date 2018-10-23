#include "include/roundRobinWithPriority.1.h"
#include "drivers/include/console.h"
#include "include/comparators.h"
#include "include/malloc.h"
/*
SCHEDULER_QUEUE* roundRobinWithPriority_newQueue(int queueQuantum,
                                                 int nextQueueID) {
    SCHEDULER_QUEUE* q = malloc(sizeof(SCHEDULER_QUEUE));
    data_RoundRobinWithPriority* data =
        malloc(sizeof(data_RoundRobinWithPriority));

    /*q->nextThreadFunction = roundRobinWithPriority_nextThread;
    q->checkEvictFunction = roundRobinWithPriority_checkEvict;
    q->restartEvictFunction = roundRobinWithPriority_restartEvict;
    q->addToQueue = roundRobinWithPriority_addToQueue;
    q->removeThread = roundRobinWithPriority_removeThread;*/
/*
    q->queueQuantum = queueQuantum;
    q->currQueueQuantum = 0;

    data->nextQueue = nextQueueID;
    data->currThreadQuantum = 0;
    data->currentMaxPriority = -1;
    data->currentPriority = 0;

    for (int i = 0; i < MAX_PRIORITY; i++) 
        data->queues[i] = linkedList_new();

    q->queueData = data;

    return q;
}


void addToQueue(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;

    int currNice = getProcessByPID(t->process)->nice;

    if(currNice == 0) {
        scheduler_enqueue(t, q->nextQueue);
        return;
    }

    linkedList_offer(data->queues[currNice - 1], t);
}

int removeThread(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;
    int status = 0;
    int currNice = getProcessByPID(t->process)->nice;

    return linkedList_removeByValue(data->queues[currNice - 1], t, 1);
}

thread_t* getThread(SCHEDULER_QUEUE* q) {
    thread_t* ret = NULL;

    data_RoundRobinWithPriority* data = q->queueData;

    int currPriority = data->maxPriority - 1;
    NODE* currQueue;

    while (ret == NULL && currPriority >= 0) {
        currQueue = data->priorityArray[currPriority];
    }

    return ret;
}

void roundRobinWithPriority_addToQueue(SCHEDULER_QUEUE* q, thread_t* t);

int roundRobinWithPriority_getMaxPriority(SCHEDULER_QUEUE* q) {
    data_RoundRobinWithPriority* data = q->queueData;

    for (int i = data->currentMaxPriority; i >= 0; i--)
        if (data->priorityArray[i] != NULL) return i;

    return -1;
}

void roundRobinWithPriority_ageThreads(SCHEDULER_QUEUE* q) {
    data_RoundRobinWithPriority* data = q->queueData;
    NODE* n;
    thread_t* t;
    for (int i = 0; i < data->currentMaxPriority; i++) {
        n = data->priorityArray[i];
        while (n != NULL) {
            t = getFirst(n);

            if (t->usedProcessor == 0) {
                // AGE
            }

            t->usedProcessor = 0;

            n = next(n);
        }
    }
}

int roundRobinWithPriority_removeThread(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;
    int status = 0;
    int currNice = getProcessByPID(t->process)->nice;

    data->priorityArray[currNice] = deleteByValue(data->priorityArray[currNice],
                                                  t, pointer_cmp, &status, 1);

    return status;
}

thread_t* roundRobinWithPriority_nextThread(SCHEDULER_QUEUE* q) {
    data_RoundRobinWithPriority* data = q->queueData;

    if (data->currentMaxPriority == -1 ||
        data->priorityArray[data->currentMaxPriority] == NULL)
        return NULL;

    thread_t* t = getFirst(data->priorityArray[data->currentMaxPriority]);

    data->priorityArray[data->currentMaxPriority] =
        deleteHead(data->priorityArray[data->currentMaxPriority]);

    if (data->currentMaxPriority == 0) {
        if (data->nextQueue != NULL) {
            data->nextQueue->addToQueue(data->nextQueue, t);
            t = roundRobinWithPriority_nextThread(q);
        }
    } else {
        getProcessByPID(t->process)->nice--;
        data->currentMaxPriority = roundRobinWithPriority_getMaxPriority(q);
    }

    roundRobinWithPriority_addToQueue(q, t);

    data->currQuantum = 0;

    t->usedProcessor++;

    return t;
}

void roundRobinWithPriority_addToQueue(SCHEDULER_QUEUE* q, thread_t* t) {
    data_RoundRobinWithPriority* data = q->queueData;

    int currNice = getProcessByPID(t->process)->nice;

    data->priorityArray[currNice] =
        insertAtEnd(data->priorityArray[currNice], t);

    if (data->currentMaxPriority < currNice)
        data->currentMaxPriority = currNice;
}

int roundRobinWithPriority_checkEvict(SCHEDULER_QUEUE* q) {
    data_RoundRobinWithPriority* data = q->queueData;

    data->currQuantum++;

    if (data->currQuantum % data->quantum == 0) {
        data->currQuantum = 0;
        return TRUE;
    }

    return FALSE;
}

void roundRobinWithPriority_restartEvict(SCHEDULER_QUEUE* q) {
    data_RoundRobinWithPriority* data = q->queueData;
    data->currQuantum =
        data->quantum -
        1;  // Garantizo que checkEvict retorne true en la próxima ejecución
}

SCHEDULER_QUEUE* roundRobinWithPriority_newQueue(int queueQuantum,
                                                 SCHEDULER_QUEUE* nextQueue) {
    SCHEDULER_QUEUE* q = malloc(sizeof(SCHEDULER_QUEUE));
    data_RoundRobinWithPriority* data =
        malloc(sizeof(data_RoundRobinWithPriority));

    q->nextThreadFunction = roundRobinWithPriority_nextThread;
    q->checkEvictFunction = roundRobinWithPriority_checkEvict;
    q->restartEvictFunction = roundRobinWithPriority_restartEvict;
    q->addToQueue = roundRobinWithPriority_addToQueue;
    q->removeThread = roundRobinWithPriority_removeThread;
    q->ageThreads = roundRobinWithPriority_ageThreads;

    q->queueQuantum = queueQuantum;
    q->currQueueQuantum = 0;

    data->nextQueue = nextQueue;
    data->quantum = queueQuantum;
    data->currQuantum = 0;
    data->currentMaxPriority = -1;

    q->queueData = data;

    return q;
}
*/