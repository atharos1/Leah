#include <memoryManagerTest.h>

static void * memories[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int i = 0;

void memoryManagerTest(int bytes) {
    if (bytes == -1) { //memories should be freed

        for (int j = 0; j < i; j++) {
          freeMemory(memories[j]);
        }
        i = 0;

    } else if ( i < 15 ) {

        void * memory;

        memory = getMemory(bytes);

        if (memory == 0) {

            printf("\n\n      Error in memory");

        } else {

            printf("\n\n      %d bytes alocados en = %Xh\n\n", bytes, memory);
            memories[i++] = memory;

        }
    } else {

        printf("\n\n      Esta permitido alocar hasta 15 bloques de memoria en el test");
        i = 0;

    }

}
