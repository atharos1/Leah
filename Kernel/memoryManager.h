#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <stdint.h>

typedef struct {
  uint64_t memoryAvailable;
  void * memoryBaseAddress;
  uint32_t maxOrder;
  uint8_t * pages;
} memoryManager_t;

#define PAGE_SIZE 4096
#define FREE 0
#define USED 1
#define PART_USED 2
#define NULL ((void *) 0)
#define OK 0
#define ERROR -1
#define TRUE 1
#define FALSE 0

#define PARENT_INDEX(currentIndex) ((currentIndex + 1) / 2 - 1)
#define LEFT_CHILD_INDEX(currentIndex) (currentIndex * 2 + 1)
#define RIGHT_CHILD_INDEX(currentIndex) (currentIndex * 2 + 2)
#define IS_LEFT_CHILD(currentIndex) (currentIndex % 2 == 1)
#define IS_RIGHT_CHILD(currentIndex) (currentIndex % 2 == 0)

void init_memoryManager(void * baseAddress, uint64_t mem_amount);
void * getMemory(uint32_t pagesToAllocate);
//Retorna OK si pudo librerar la memoria, ERROR en caso contrario
uint32_t freeMemory(void * addressToFree);

#endif
