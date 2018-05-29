#include <stdint.h>
#include <interruptions/defs.h>
#include <interruptions/intHandlers.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>

/* Assembly functions */
void _sti();
void _cli();
void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);

/* Handlers (Assembly) */
void _ex00Handler();

void _irq00Handler();
void _irq01Handler();
void _int80handler();

/* Descriptor de interrupcion */
typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)		/* Reestablece la alinceación actual */

DESCR_INT * idt = (DESCR_INT *) 0;	// IDT de 255 entradas

static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}

int s(int i);

void writeIDT() {
	_cli();	

	setup_IDT_entry (0x0, (uint64_t)&_ex00Handler);

	setup_IDT_entry (0x20, (uint64_t)&_irq00Handler);
	timerRestart(); //Inicializa el timer_Tick

	
	setup_IDT_entry (0x21, (uint64_t)&_irq01Handler);
	setup_IDT_entry (0x80, (uint64_t)&_int80handler);
  	//setup_IDT_entry (0x00, (uint64_t)&_exception0Handler);

	//Habilita 1 y 2 (Timer_Tick y teclado)
	picMasterMask(0xFC); 
	picSlaveMask(0xFF);

	//appendFunctionToTimer(cursorTick, 10); //Esto no va acá, bue
	_sti();
}

void irqDispatcher(int n) {
	switch(n) {
		case 0: //Timer_Tick
			timerTick();
			break;
		case 1: //Keyboard
			kb_fetch();
			break;
	}
}

void exDispatcher(int n) {
	switch(n) {
		case 0: //Division by 0
			clearScreen();
			printf("\n\nEXCEPTION: DIVISION BY ZERO\n\n");
			while(1)
				_halt();
			break;
	}
}