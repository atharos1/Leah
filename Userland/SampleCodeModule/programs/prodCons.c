#include "prodCons.h"
#include <../StandardLibrary/mutex.h>
#include <../StandardLibrary/sem.h>
#include <../StandardLibrary/pthread.h>
#include <../StandardLibrary/stdio.h>

char trays[TRAYS_QTY + 1];
int full;
int empty;
int mutex;
int indexChef = 0, indexWaiter = 0;
pthread_t waiters[MAX_PRODCONS] = {0};
pthread_t chefs[MAX_PRODCONS] = {0};
int waitersQty = 0, chefsQty = 0;

void prodcons() {
  printMenu();
  mutex_create("prodConsMutex");
  mutex = mutex_open("prodConsMutex");
	sem_create("fullSem", TRAYS_QTY);
  full = sem_open("fullSem");
  sem_create("emptySem", 0);
  empty = sem_open("emprySem");
	initTrays(trays);

	printTrays(trays);

	int c;
	while((c = getchar()) != QUIT) {
		switch(c) {
			case INC_CHEF:
				callChef();
				break;
			case DEC_CHEF:
				restChef();
				break;
			case INC_WAITER:
				callWaiter();
				break;
			case DEC_WAITER:
				restWaiter();
				break;
			default:
				break;
		}
	}

	terminateAll();
  mutex_delete("prodConsMutex");
  sem_delete("fullSem");
  sem_delete("emptySem");
	printf("La simulacion termino\n");
}

void printMenu() {
  printf("\nEl restaurante Leah tiene un maximo de 20 bandejas\n");
	printf("Hay 10 chefs y 10 camareros que inicialmente estan descansando\n\n", MAX_PRODCONS, MAX_PRODCONS);
	printf("Inserte:\n");
  printf("    %c para llamar a un chef\n", INC_CHEF);
	printf("    %c para poner a descansar a un chef\n", DEC_CHEF);
	printf("    %c para llamar a un camarero\n", INC_WAITER);
	printf("    %c para poner a descansar a un camarero\n", DEC_WAITER);
	printf("    %c para terminar la simulacion\n\n\n", QUIT);
}

void initTrays(char * trays) {
	for (int i = 0; i < TRAYS_QTY; i++) {
		trays[i] = EMPTY_SPACE;
	}
	trays[TRAYS_QTY] = 0;
}

void printTrays(char * trays) {
	printf("Trays = ");
	printf(trays);
	printf("\n\n\n");
}

void callChef() {
  mutex_lock(mutex);
	if(chefsQty >= MAX_PRODCONS) {
		printf("No hay mas chefs descansando\n");
		mutex_unlock(mutex);
		return;
	}
	chefs[chefsQty++] = pthread_create(&chef, (void*) 0);
	mutex_unlock(mutex);
}

void * chef(void * args) {
	printf("Soy un chef que se pone a trabajar!\n");
	while(1) {
		sem_wait(full);
		mutex_lock(mutex);

    trays[indexChef++] = '0';
    indexWaiter++;
		printTrays(trays);

		sem_signal(empty);
		mutex_unlock(mutex);
	}
	return 0;
}

void restChef() {
	mutex_lock(mutex);
	if(chefsQty == 0) {
		printf("No hay chefs trabajando\n");
		mutex_unlock(mutex);
		return;
	}
	chefsQty--;
	pthread_cancel(chefs[chefsQty]);

	printf("Un chef se fue a descansar\n");
	mutex_unlock(mutex);
}

void callWaiter() {
	mutex_lock(mutex);
	if(waitersQty >= MAX_PRODCONS) {
		printf("No hay mas camareros descansando\n");
		mutex_unlock(mutex);
		return;
	}
	waiters[waitersQty++] = pthread_create(&waiter, (void*) 0);
	mutex_unlock(mutex);
}

void * waiter(void * args) {
	printf("Soy un camarero que se pone a trabajar!\n");
	while(1) {
		sem_wait(empty);
		mutex_lock(mutex);

		trays[indexWaiter--] = EMPTY_SPACE;
    indexChef--;
		printTrays(trays);

		sem_signal(full);
		mutex_unlock(mutex);
	}
}

void restWaiter() {
	mutex_lock(mutex);
	if(waitersQty == 0) {
		printf("No hay camareros trabajando\n");
		mutex_unlock(mutex);
		return;
	}
  waitersQty--;
  pthread_cancel(waiters[waitersQty]);

	printf("Un camarero se fue a descansar\n");
	mutex_unlock(mutex);
}

void terminateAll() {
  mutex_lock(mutex);
	for (int i = 0; i < chefsQty; i++) {
			pthread_cancel(chefs[i]);
	}
  for (int i = 0; i < waitersQty; i++) {
      pthread_cancel(waiters[i]);
  }
  mutex_unlock(mutex);
}
