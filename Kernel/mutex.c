#define LOCKED 1;
#define UNLOCKED 0;
#include "malloc.h"
#include "circularList.h"
#include "asm/libasm.h"

typedef struct {
	int state;
	NODE * queue;
} mutex_struct;

typedef mutex_struct * mutex;

mutex mutex_init() {
	mutex m = malloc(sizeof(mutex_struct));
	m->state = UNLOCKED;
	//m->queue =
	return m;
}

void lock(mutex m, int pid) {
	_acquire_lock(m->state, pid);
}

// Called from ASM
void addProcessToQueue(mutex m, int pid) {
	// Add pid to end of queue
	// Block process with PID: pid
}

void unlock(mutex m) {
	if(m->state == UNLOCKED) {
		return;
	}
	m->state = UNLOCKED;
	if(length(mutex->queue) > 0) {
		// Extract head of queue
		//call_next_process(mutex);
	}
}