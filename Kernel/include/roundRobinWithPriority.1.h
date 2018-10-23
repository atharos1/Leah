#ifndef ROUND_ROBIN_WITH_PRIORITY1_H
#define ROUND_ROBIN_WITH_PRIORITY1_H

#include "include/linkedList.h"
#include "scheduler.h"

#define MAX_PRIORITY 5

typedef struct {
    int quantum;
    int currQuantum;
    SCHEDULER_QUEUE* nextQueue;
    NODE* priorityArray[MAX_PRIORITY];
    int currentMaxPriority;
    int currThreadQuantum;
    int currentPriority;
    linkedList_t queues[MAX_PRIORITY];
} data_RoundRobinWithPriority;

/*SCHEDULER_QUEUE* roundRobinWithPriority_newQueue(int quantum,
                                                 SCHEDULER_QUEUE* nextQueue);*/

#endif