#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "scheduler.h"

typedef struct {
    NODE* queue;
} data_RoundRobin;

SCHEDULER_QUEUE* roundRobin_newQueue(int queueQuantum, int nextQueueID);

#endif
