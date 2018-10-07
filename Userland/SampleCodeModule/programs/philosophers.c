#include "include/philosophers.h"
#include "../StandardLibrary/include/mutex.h"
#include "../StandardLibrary/include/sem.h"
#include "../StandardLibrary/include/pthread.h"
#include "../StandardLibrary/include/stdio.h"

int pMutex;
int philosophersQty, forks, philosophersToDie;
pthread_t philosophers[MAX_PHI] = {0};

int philosophers() {
  printMenu();
  mutex_create("philosophersMutex");
  pMutex = mutex_open("philosophersMutex");
  philosophersQty = 0;
  philosophersToDie = 0;
  forks = 0;

	printTable();

	int c;
	while((c = getchar()) != QUIT) {
		switch(c) {
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

	terminateAll();
  mutex_delete("philosophersMutex");
	printf("La simulacion termino\n");
	return 0;
}

void printMenu() {
  printf("\nEn la comunidad Leah hay un maximo de %d filosofos por nacer\n", MAX_PHI);
	printf("Inserte:\n");
  printf("    %c para que un filosofo nazca\n", INC);
	printf("    %c para que un filosofo muera\n", DEC);
	printf("    %c para terminar la simulacion\n\n\n", QUIT);
}

void printTable() {

}

void born() {
  mutex_lock(pMutex);
	if(philosophersQty >= MAX_PHI) {
		printf("No hay mas filosofos por nacer\n\n\n");
		mutex_unlock(pMutex);
		return;
	}
  forks++;
	philosophers[philosophersQty++] = pthread_create(&philosopher, (void*) 0);
	mutex_unlock(pMutex);
}

void * philosopher(void * args) {
	printf("Soy un filosofo que acaba de nacer!\n\n\n");
	while(1) {
		mutex_lock(pMutex);
    if (philosophersToDie > 0) {
      philosopherSuicide();
    }
    // plates ++;
    // sem_signal(empty);
    //
		// printTrays();
    //
		// mutex_unlock(mutex);
	}
	return 0;
}

void die() {
	mutex_lock(pMutex);
  if(philosophersQty == 0) {
		printf("No hay filosofos vivos\n\n\n");
		mutex_unlock(pMutex);
		return;
	}
  philosophersToDie ++;
  mutex_unlock(pMutex);
}

void philosopherSuicide() {
  philosophersQty--;
  philosophersToDie--;
  forks--;
  pthread_t phiToDie = philosophers[philosophersQty];
  mutex_unlock(pMutex);
  printf("Un filosofo se murio\n\n\n");
  pthread_cancel(phiToDie);
}

void terminateAll() {
  mutex_lock(pMutex);
	for (int i = 0; i < philosophersQty; i++) {
			pthread_cancel(philosophers[i]);
	}
  mutex_unlock(pMutex);
}
