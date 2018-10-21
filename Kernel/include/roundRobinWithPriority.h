#ifndef ROUND_ROBIN_WITH_PRIORITY_H
#define ROUND_ROBIN_WITH_PRIORITY_H

#include "scheduler.h"

#define MAX_PRIORITY 30

typedef struct {
    int quantum;
    int currQuantum;
    SCHEDULER_QUEUE * nextQueue;
    NODE * priorityArray[MAX_PRIORITY];
    int currentMaxPriority;
} data_RoundRobinWithPriority;

SCHEDULER_QUEUE * roundRobinWithPriority_newQueue(int quantum, SCHEDULER_QUEUE * nextQueue);
void roundRobinWithPriority_yield(SCHEDULER_QUEUE * q);

#endif