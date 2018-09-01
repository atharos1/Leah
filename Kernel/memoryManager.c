#include <memoryManager.h>

/////// VARIABLES GLOBALES ///////

static memoryManager_t memoryManager;

/////// DECLARACION FUNCIONES ///////

static uint32_t getMaxOrder();
static uint64_t getNodeQuantity();
static void init_pagesStatus();
static uint32_t pagesQuantity();
static uint64_t getOffsetFromBaseAddress(uint64_t bytesToAllocate);
static int nextPagesIndexToSearch(int pagesIndex, uint32_t * currentOrder, uint64_t * currentBlockSize);
static uint64_t calculateOffset(int pagesIndex, uint32_t order);
static void markUsedNodes(int pagesIndex);
static uint32_t getBuddyIndex(uint32_t currentIndex);
static uint64_t pageSizeMultiple(uint64_t bytesToAllocate);
static uint64_t getSkippedPages(void * addressToFree);

/////// CODIGO ///////

void init_memoryManager(void * baseAddress, uint64_t mem_amount) {
  memoryManager.memoryAvailable = mem_amount;
  // Los primeros NodeQuantity bytes son reservados para poner el estado de cada nodo
  // Se acceden mediante pages que empieza en baseAddress
  memoryManager.memoryBaseAddress = (void *)((char *)baseAddress + getNodeQuantity());
  memoryManager.maxOrder = getMaxOrder();
  memoryManager.pages = baseAddress;

  init_pagesStatus();
}

//Todas las paginas se setean en FREE
static void init_pagesStatus() {
  for (int i = 0; i < getNodeQuantity(); i++) {
    memoryManager.pages[i] = FREE;
  }
}

//Retorna el puntero a la zona de memoria asignada o NULL en caso de error
void * getMemory(uint32_t bytesToAllocate) {

  if (bytesToAllocate <= 0 || bytesToAllocate > memoryManager.memoryAvailable) {
  return NULL;
  }

  uint64_t offset = getOffsetFromBaseAddress(bytesToAllocate);
  if (offset == ERROR) {
    return NULL;
  }

  return (void *)((uint64_t)memoryManager.memoryBaseAddress + offset);
}

static uint32_t pagesQuantity() {
  return memoryManager.memoryAvailable / PAGE_SIZE;
}

static uint64_t getOffsetFromBaseAddress(uint64_t bytesToAllocate) {

  int pagesIndex = 0;
  uint32_t currentOrder = 0;
  uint64_t currentBlockSize = memoryManager.memoryAvailable;
  uint64_t neededBlockSize = pageSizeMultiple(bytesToAllocate);
  uint8_t memoryAllocated = FALSE;

  while (pagesIndex >= 0 && !memoryAllocated) {

    if (memoryManager.pages[pagesIndex] != USED) {

      if (neededBlockSize == currentBlockSize) {

          if (memoryManager.pages[pagesIndex] == FREE) {

              memoryManager.pages[pagesIndex] = USED;
              markUsedNodes(pagesIndex);
              memoryAllocated = TRUE;

          } else {

              pagesIndex = nextPagesIndexToSearch(pagesIndex, &currentOrder, &currentBlockSize);
              if (pagesIndex < 0) {
                return ERROR;
              }

          }

      } else {

        currentOrder ++;
        currentBlockSize /= 2;

        if (memoryManager.pages[LEFT_CHILD_INDEX(pagesIndex)] != USED) {

          pagesIndex = LEFT_CHILD_INDEX(pagesIndex);

        } else {

          pagesIndex = RIGHT_CHILD_INDEX(pagesIndex);
        }
      }
    } else {
      pagesIndex = nextPagesIndexToSearch(pagesIndex, &currentOrder, &currentBlockSize);
      if (pagesIndex < 0) {
        return ERROR;
      }
    }
  }

  return (memoryAllocated) ? calculateOffset(pagesIndex, currentOrder) : ERROR;
}

static int nextPagesIndexToSearch(int pagesIndex, uint32_t * currentOrder, uint64_t * currentBlockSize) {
  if (IS_LEFT_CHILD(pagesIndex)) {

    pagesIndex ++;

  } else {

    while (IS_RIGHT_CHILD(pagesIndex) && pagesIndex >= 0) {
      (*currentOrder)--;
      (*currentBlockSize) *= 2;
      pagesIndex = PARENT_INDEX(pagesIndex);
    }

  }
  return pagesIndex;
}

static uint64_t calculateOffset(int pagesIndex, uint32_t order) {
  uint64_t skippedPages = (pagesIndex + 1 - (1 << order)) << (memoryManager.maxOrder - order);
  return skippedPages * PAGE_SIZE;
}

static void markUsedNodes(int pagesIndex) {
  if (pagesIndex > 0 && memoryManager.pages[getBuddyIndex(pagesIndex)] == USED) {
    pagesIndex = PARENT_INDEX(pagesIndex);

    if (pagesIndex >= 0) {
      memoryManager.pages[pagesIndex] = USED;
    }
    markUsedNodes(pagesIndex);
  } else {
    while (pagesIndex > 0) {
      pagesIndex = PARENT_INDEX(pagesIndex);
      memoryManager.pages[pagesIndex] = PART_USED;
    }
  }
}

static uint32_t getBuddyIndex(uint32_t currentIndex) {
  if (IS_LEFT_CHILD(currentIndex)) {
    return currentIndex + 1;
  } else {
    return currentIndex - 1;
  }
}

// Redondea los bytes para alocar al tamaño de los bloques permitidos
static uint64_t pageSizeMultiple(uint64_t bytesToAllocate) {
  uint64_t multiple = PAGE_SIZE;

  if (bytesToAllocate > PAGE_SIZE) {
    while (multiple <= memoryManager.memoryAvailable && bytesToAllocate > multiple) {
      multiple *= 2;
    }
  }

  return multiple;
}

uint32_t freeMemory(void * addressToFree) {
  uint64_t skippedPages = getSkippedPages(addressToFree);
  uint64_t pagesPerOrder = pagesQuantity();
  uint32_t pagesIndex = 0;
  uint32_t pagesSkippedAtBranch = 0;
  uint64_t nodeQuantity = getNodeQuantity();
  uint32_t leftChildIndex, rightChildIndex;
  uint8_t found = FALSE;

  if (skippedPages >= pagesQuantity()) {
    return ERROR;
  }

  while (found == FALSE && pagesIndex < nodeQuantity) {
    leftChildIndex = LEFT_CHILD_INDEX(pagesIndex);
    rightChildIndex = RIGHT_CHILD_INDEX(pagesIndex);

    if (memoryManager.pages[pagesIndex] == USED) {
      memoryManager.pages[pagesIndex] = FREE;

      if (leftChildIndex < nodeQuantity && rightChildIndex < nodeQuantity
          && memoryManager.pages[leftChildIndex] == FREE
          && memoryManager.pages[rightChildIndex] == FREE) {
            return OK;
      }
    }

    pagesPerOrder /= 2;

    if (skippedPages < pagesSkippedAtBranch + pagesPerOrder) {
      pagesIndex = leftChildIndex;
    } else {
      pagesSkippedAtBranch += pagesPerOrder;
      pagesIndex = RIGHT_CHILD_INDEX(pagesIndex);
    }
  }

  return ERROR;
}

static uint64_t getSkippedPages(void * addressToFree) {
  return (((uint64_t) addressToFree - (uint64_t) memoryManager.memoryBaseAddress) / PAGE_SIZE);
}

static uint32_t log2(uint32_t number) {
	uint32_t power = 0;
	while(number - 1 > 0) {
		number /= 2;
		power ++;
	}
	return power;
}

// Retorna la altura maxima del arbol dependiendo de la cantidad de memoria y el tamaño de pagina
static uint32_t getMaxOrder() {
  return log2(memoryManager.memoryAvailable) - log2(PAGE_SIZE);
}

static uint64_t getNodeQuantity() {
  return (1 << (getMaxOrder() + 1)) - 1;
}
