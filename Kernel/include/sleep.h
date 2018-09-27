#ifndef SLEEP_H
#define SLEEP_H

#include "process.h"
#include "memoryManager.h"

typedef struct sleepingThreadNode {
    thread_t * thread;
    int ticks_remaining;
    struct sleepingThreadNode * next;
} sleepingThreadNode;

void sleep(thread_t * thread, int millis);
void sleep_update();
void sleepCurrentThread(int millis);

#endif