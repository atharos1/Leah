#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include "scheduler.h"

typedef struct {
    int quantum;
    int currQuantum;
} data_RoundRobin;

SCHEDULER_QUEUE * roundRobin_newQueue(int quantum);

#endif
