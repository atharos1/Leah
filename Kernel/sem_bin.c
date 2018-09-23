#include "sem.h"

sem_bin_signal(sem_t sem) {
    if(sem->value == 0)
        sem_signal(sem);
}

sem_bin_wait(sem_t sem) {
    sem_wait(sem);
}