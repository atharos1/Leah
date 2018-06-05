#ifndef LIBASM_H
#define LIBASM_H
#include <stdint.h>

unsigned char _readKey();

unsigned int _RTC(int whatToGet);

/* Beeper functions*/
void _beep_start(uint16_t freq);
void _beep_stop();

void _halt();
void * _rsp();
char * _cpuVendor(char *);

void _sti();
void _cli();
void _picMasterMask(uint8_t mask);
void _picSlaveMask(uint8_t mask);

/* Handlers */
void _ex00Handler();
void _ex06Handler();

void _irq00Handler();
void _irq01Handler();
void _int80handler();

#endif


