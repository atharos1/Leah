#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>

unsigned char _readKey();

unsigned int _RTC(int whatToGet);

void _halt();
void * _rsp();
char * _cpuVendor(char *);

void _sti();
void _cli();
void _picMasterMask(uint8_t mask);
void _picSlaveMask(uint8_t mask);

/* Beeper functions*/
void _beep_start(uint16_t freq);
void _beep_stop();

/* Handlers */
void _ex00Handler();
void _ex06Handler();

void _irq00Handler();
void _irq01Handler();
void _int80handler();

/* Scheduler */
void * _initialize_stack_frame(void * wrapper, void * rip, void * rsp, void * args);
void _force_scheduler();

void _sem_increment();
void _sem_decrement();
int _mutex_acquire(int * lockValue);

#endif
