#include "include/philosophers.h"
#include "../StandardLibrary/include/integerList.h"
#include "../StandardLibrary/include/mutex.h"
#include "../StandardLibrary/include/pthread.h"
#include "../StandardLibrary/include/sem.h"
#include "../StandardLibrary/include/stdio.h"
#include "../StandardLibrary/include/stdlib.h"

typedef struct philos {
    int id;
    pthread_t phi;
} philos_t;

void printMenuPhi();
void terminateAllPhi();

int pMutex, eMutex;
int philosophersQty, forks, philosophersToDie, phiInTable, id, indexToDie;
philos_t phis[MAX_PHI] = {0};
Node *thinkingList, *eatingList;

int philosophers() {
    printMenuPhi();
    mutex_create("philosophersMutex");
    pMutex = mutex_open("philosophersMutex");
    mutex_create("eatingMutex");
    eMutex = mutex_open("eatingMutex");
    philosophersQty = 0;
    sem_create("forks", 0);
    forks = sem_open("forks");
    sem_create("inTable", 0);
    phiInTable = sem_open("inTable");
    philosophersToDie = 0;
    id = 0;
    indexToDie = 0;

    thinkingList = integerList_init();
    eatingList = integerList_init();

    int c;
    while ((c = getchar()) != QUIT) {
        switch (c) {
            case INC:
                born();
                break;
            case DEC:
                die();
                break;
            default:
                break;
        }
    }

    terminateAllPhi();
    mutex_delete("philosophersMutex");
    mutex_delete("eatingMutex");
    sem_delete("forks");
    sem_delete("inTable");
    printf("La simulacion termino\n");
    return 0;
}

void printMenuPhi() {
    printf("\nEn la comunidad Leah hay un maximo de %d filosofos por nacer\n",
           MAX_PHI);
    printf("Inserte:\n");
    printf("    %c para que un filosofo nazca\n", INC);
    printf("    %c para que un filosofo muera\n", DEC);
    printf("    %c para terminar la simulacion\n\n", QUIT);
}

void born() {
    mutex_lock(pMutex);
    if (philosophersQty >= MAX_PHI) {
        printf("No hay mas filosofos por nacer\n\n\n");
        mutex_unlock(pMutex);
        return;
    }
    sem_signal(forks);
    uint64_t index = id++;
    philos_t phi = {};
    phi.phi = pthread_create(&philosopher, (void*)index);
    phi.id = index;
    indexToDie = id - 1;
    phis[philosophersQty++] = phi;
    if (philosophersQty > 1) sem_signal(phiInTable);
    mutex_unlock(pMutex);

    printf("\nSoy el filosofo %d que acaba de nacer!\n\n", index);

    mutex_lock(eMutex);
    thinkingList = insertElement(thinkingList, index);
    mutex_unlock(eMutex);
}

void* philosopher(void* args) {
    int i = (uint64_t)args;

    while (1) {
        if (philosophersToDie > 0 && i == indexToDie) {
            philosopherSuicide();
        }

        sem_wait(phiInTable);
        sem_wait(forks);
        sem_wait(forks);

        mutex_lock(eMutex);
        thinkingList = deleteElement(thinkingList, i);
        eatingList = insertElement(eatingList, i);
        printStatus();
        mutex_unlock(eMutex);

        sys_sleep(1000);
        sem_signal(forks);
        sem_signal(forks);
        sem_signal(phiInTable);

        mutex_lock(eMutex);
        eatingList = deleteElement(eatingList, i);
        thinkingList = insertElement(thinkingList, i);
        mutex_unlock(eMutex);
    }
    return 0;
}

void die() {
    mutex_lock(pMutex);
    if (philosophersQty == 0) {
        printf("No hay filosofos vivos\n\n");
        mutex_unlock(pMutex);
        return;
    }
    philosophersToDie++;
    mutex_unlock(pMutex);
}

void philosopherSuicide() {
    mutex_lock(pMutex);
    philosophersQty--;
    philosophersToDie--;
    indexToDie = (phis[philosophersQty - 1]).id;
    pthread_t phiToDie = (phis[philosophersQty]).phi;

    mutex_unlock(pMutex);
    sem_wait(phiInTable);
    sem_wait(forks);
    printf("\nEl filosofo %d ha muerto!\n\n", (phis[philosophersQty]).id);
    mutex_lock(eMutex);
    thinkingList = deleteElement(thinkingList, (phis[philosophersQty]).id);
    mutex_unlock(eMutex);
    pthread_cancel(phiToDie);
}

void terminateAllPhi() {
    mutex_lock(pMutex);
    for (int i = 0; i < philosophersQty; i++) {
        pthread_cancel((phis[i]).phi);
    }

    integerList_destroy(eatingList);
    integerList_destroy(thinkingList);

    mutex_unlock(pMutex);
}

void printStatus() {
    int spacesE = countSpaces(eatingList);

    printf("Comiendo: ");
    printList(eatingList);

    for (int i = 0; i < (3 * MAX_PHI / 2) - spacesE; i++) {
        printf(" ");
    }

    printf("Pensando: ");
    printList(thinkingList);

    printf("\n");
}
