#ifndef STD_LIB_H
#define STD_LIB_H

#include <stdint.h>

#define NULL ((void *) 0)

void *malloc(uint32_t size);
void free(void *pointer);

#endif
