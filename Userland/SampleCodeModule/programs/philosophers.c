#include "include/philosophers.h"
#include "../StandardLibrary/include/mutex.h"
#include "../StandardLibrary/include/sem.h"
#include "../StandardLibrary/include/pthread.h"
#include "../StandardLibrary/include/stdio.h"

typedef struct philos {
  int id;
  pthread_t phi;
} philos_t;

void printMenuPhi();
void terminateAllPhi();

int pMutex, eMutex;
int philosophersQty, forks, philosophersToDie, phiInTable, id, eatingQty;
philos_t phis[MAX_PHI] = {0};

int philosophers() {
  printMenuPhi();
  mutex_create("philosophersMutex");
  pMutex = mutex_open("philosophersMutex");
  philosophersQty = 0;
  sem_create("forks", 0);
  forks = sem_open("forks");
  sem_create("inTable", 0);
  phiInTable = sem_open("inTable");
  philosophersToDie = 0;
  id = 0;
  mutex_create("eatingMutex");
  eMutex = mutex_open("eatingMutex");
  eatingQty = 0;

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

	terminateAllPhi();
  mutex_delete("philosophersMutex");
  sem_delete("forks");
  sem_delete("inTable");
	printf("La simulacion termino\n");
	return 0;
}

void printMenuPhi() {
  printf("\nEn la comunidad Leah hay un maximo de %d filosofos por nacer\n", MAX_PHI);
	printf("Inserte:\n");
  printf("    %c para que un filosofo nazca\n", INC);
	printf("    %c para que un filosofo muera\n", DEC);
	printf("    %c para terminar la simulacion\n\n", QUIT);
}

void born() {
  mutex_lock(pMutex);
	if(philosophersQty >= MAX_PHI) {
		printf("No hay mas filosofos por nacer\n\n\n");
		mutex_unlock(pMutex);
		return;
	}
  sem_signal(forks);
  int index = id++;
  philos_t phi = {};
  phi.phi = pthread_create(&philosopher, (void*)index);
  phi.id = index;
	phis[philosophersQty++] = phi;
  if (philosophersQty>1)
    sem_signal(phiInTable);
	mutex_unlock(pMutex);
}

void * philosopher(void * args) {
  int i = (int)args;
	printf("\nSoy el filosofo %d que acaba de nacer!\n\n",i);
	while(1) {

    if (philosophersToDie > 0) {
      philosopherSuicide();
    }
    sem_wait(phiInTable);
    sem_wait(forks);
    sem_wait(forks);
    //comer();
    mutex_lock(eMutex);
    eatingQty++;
    mutex_unlock(eMutex);
    if(i < 10)
      printf("El filosofo %d esta comiendo. \t",i);
    else
      printf("El filosofo %d esta comiendo.\t",i);
    printStatus();
    sys_sleep(1000);
    sem_signal(forks);
    sem_signal(forks);
    sem_signal(phiInTable);

    //pensar();
    mutex_lock(eMutex);
    eatingQty--;
    mutex_unlock(eMutex);
    //printf("El filosofo %d esta pensando.\n",i);
    //printStatus();
	}
	return 0;
}

void die() {
	mutex_lock(pMutex);
  if(philosophersQty == 0) {
		printf("No hay filosofos vivos\n\n");
		mutex_unlock(pMutex);
		return;
	}
  philosophersToDie ++;
  mutex_unlock(pMutex);
}

void philosopherSuicide() {
  mutex_lock(pMutex);
  philosophersQty--;
  philosophersToDie--;
  pthread_t phiToDie = (phis[philosophersQty]).phi;
  mutex_unlock(pMutex);
  printf("\nEl filosofo %d ha muerto!\n\n",(phis[philosophersQty]).id);
  sem_wait(forks);
  sem_wait(phiInTable);
  pthread_cancel(phiToDie);
}

void terminateAllPhi() {
  mutex_lock(pMutex);
	for (int i = 0; i < philosophersQty; i++) {
			pthread_cancel((phis[i]).phi);
	}
  mutex_unlock(pMutex);
}

void printStatus() {
  int n;
  printf("Comiendo: ");
  for(n = 0; n < eatingQty; n++) {
    printf("*");
  }
  for(n = eatingQty; n < 12; n++) {
    printf(" ");
  }
  printf("Pensando: ");
  for(n = eatingQty; n < philosophersQty; n++) {
    printf("*");
  }
  printf("\n");
}
