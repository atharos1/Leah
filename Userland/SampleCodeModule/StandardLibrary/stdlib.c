#include "stdlib.h"
#include "../asm/asmLibC.h"

#define FREE 0
#define USED 1

typedef struct block {
  uint32_t size;
  struct block *next;
  struct block *prev;
  int state;
} block_t;

static void split(block_t *block, uint32_t size);
static void merge(block_t *block);

void init_malloc(block_t * heapBase) {
  heapBase->size = sys_getHeapSize() - sizeof(block_t);
  heapBase->prev = NULL;
  heapBase->next = NULL;
  heapBase->state = FREE;
}

void * malloc(uint32_t size) {

  block_t * heapBase = sys_getHeapBase();
  if(heapBase->size == 0)
    init_malloc(heapBase);

  block_t *currBlock = heapBase;
  while(currBlock->state == USED || currBlock->size < size) {
    if (currBlock->next == NULL)
      return NULL;
    currBlock = currBlock->next;
  }

  split(currBlock, size);
  currBlock->state = USED;

  return (void*)(currBlock+1);
}

void free(void *pointer) {
  merge(((block_t*)pointer)-1);
}

static void split(block_t *block, uint32_t size) {
  if (block->size - size > sizeof(block_t)) {
    block_t *aux = block->next;
    block->next = (block_t*)((char*)(block+1) + size);
    block->next->size = block->size - size - sizeof(block_t);
    block->size = size;
    block->next->next = aux;
    block->next->prev = block;
    block->next->state = FREE;
  }
}

static void merge(block_t *block) {
  block->state = FREE;

  if(block->next != NULL && block->next->state == FREE) {
    block->size += block->next->size + sizeof(block_t);
    block->next = block->next->next;
    block->next->prev = block;
  }

  if (block->prev != NULL && block->prev->state == FREE)
    merge(block->prev);
}
