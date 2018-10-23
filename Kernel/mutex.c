#include "include/mutex.h"
#include "asm/libasm.h"
#include "include/fileSystem.h"
#include "include/malloc.h"
#include "include/scheduler.h"

mutex_t mutex_create() {
    mutex_t m = malloc(sizeof(mutex_struct));
    m->value = 0;
    // m->owner = NULL;
    m->ownerPID = m->ownerTID = -1;
    m->lockedQueue = NULL;
    return m;
}

void mutex_delete(mutex_t mutex) { free(mutex); }

void mutex_lock(mutex_t mutex) {
    if (getCurrentThread() == NULL) return;

    if (!_mutex_acquire(&(mutex->value))) {
        mutex->ownerPID = getCurrentPID();
        mutex->ownerTID = getCurrentThread()->tid;
        // mutex->owner = getCurrentThread();
    } else {
        thread_t* thread = getCurrentThread();
        mutex->lockedQueue = insertAtEnd(mutex->lockedQueue, thread);
        scheduler_dequeue_current();
        _force_scheduler();
    }
}

void mutex_unlock(mutex_t mutex) {
    /*if (getCurrentThread() == NULL || mutex->owner != getCurrentThread() )
      return;*/

    if (getCurrentThread() == NULL || mutex->ownerPID != getCurrentPID() ||
        mutex->ownerTID != getCurrentThread()->tid)
        return;

    while (mutex->lockedQueue != NULL) {
        thread_t* t = getFirst(mutex->lockedQueue);
        mutex->lockedQueue = deleteHead(mutex->lockedQueue);

        if (t->status == DEAD)
            eraseTCB(t);
        else {
            // mutex->owner = t;
            mutex->ownerPID = t->process;
            mutex->ownerTID = t->tid;
            scheduler_enqueue(t, 0);
            return;
        }
    }
    mutex->value = 0;

    // printf("Libero mutex");
}

// void mutex_unlock(mutex_t mutex) {
//   if (getCurrentThread() == NULL)
//     return;
//   if( mutex->owner == getCurrentThread() ) {
//     thread_t * t = getFirst(mutex->lockedQueue);
//     mutex->owner = t;
//     if (t == NULL) {
//       mutex->value = 0;
//     } else {
//       mutex->lockedQueue = deleteHead(mutex->lockedQueue);
//       scheduler_enqueue(t);
//     }
//   }
// }
