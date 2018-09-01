#include <memoryManagerTest.h>

static void * memories[10] = {0,0,0,0,0,0,0,0,0,0};
static int i = 0;

void memoryManagerTest(int pages) {
    if (pages == -1) { //memories should be freed

        for (int j = 0; j < i; j++) {
          freeMemory(memories[j]);
        }
        i = 0;

    } else {

        void * memory;

        memory = getMemory(pages);

        if (memory == 0) {

            printf("Error in memory");

        } else {

            printf("\n\n      %d bytes alocados en = %Xh\n\n", pages * PAGE_SIZE, memory);
            memories[i++] = memory;

      }
    }

}
