#ifndef MODULELOADER_H
#define MODULELOADER_H

void loadModules(void * payloadStart, void ** moduleTargetAddress);
extern uint32_t userlandSize;

#endif
