#include <stdint.h>

#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

/*extern uint64_t stackPointerBackup;
extern uint64_t instructionPointerBackup;*/

typedef int (*EntryPoint)();
static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

#endif