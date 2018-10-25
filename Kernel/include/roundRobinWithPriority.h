#ifndef ROUND_ROBIN_WITH_PRIORITY1_H
#define ROUND_ROBIN_WITH_PRIORITY1_H

#include "linkedList.h"
#include "scheduler.h"

#define MAX_PRIORITY 5

typedef struct {
    int currentPriority;
    linkedList_t queues[MAX_PRIORITY];
    linkedList_t backupQueue;
} data_RoundRobinWithPriority;

SCHEDULER_QUEUE* roundRobinWithPriority_newQueue(int queueQuantum,
                                                 int nextQueueID);

#endif