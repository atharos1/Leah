#include <stdint.h>
#include "defs.h"
#include "../drivers/include/timer.h"
#include "../drivers/include/kb_driver.h"
#include "../drivers/include/console.h"
#include "../drivers/include/speaker.h"
#include "../asm/libasm.h"
#include "../interruptions/idt.h"
#include "../include/process.h"
#include "../include/scheduler.h"

void dumpData(char * msg, uint64_t * RIP, uint64_t * RSP);


unsigned char inportb(unsigned short portid) {
	return _inportb(portid);
}

void outportb(unsigned short portid, unsigned char value) {
	_outportb(portid, value);
}

/* Descriptor de interrupcion */
typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

//#pragma pack(pop)		/* Reestablece la alinceación actual */

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


void kernelPanic(char * msg, uint64_t * RIP, uint64_t * RSP) {
	setFontColor(0x000000);
	setBackgroundColor(0xDC143C);

	clearScreen();

	setFontSize(15);

	printf(":D");

	setFontSize(8);
	incLine(2);

	printf("Se acabo todo.\nTodo todillo.\n");
	setFontSize(4);
	incLine(4);
	printf("Que mal...\n");

	setFontSize(1);
	incLine(31);
	dumpData(msg, RIP, RSP);

	while(1)
		_halt();

}

#define PIT_CHANNEL0 0x40  //PIT Channel 0's Data Register Port
#define PIT_CHANNEL1 0x41  //PIT Channels 1's Data Register Port, we wont be using this here
#define PIT_CHANNEL2 0x42  //PIT Channels 2's Data Register Port
#define PIT_CMDREG 0x43  //PIT Chip's Command Register Port

void writeIDT() {
	_cli();

	setup_IDT_entry (0x0, (uint64_t)&_ex00Handler); //DivByZero
	setup_IDT_entry (0x6, (uint64_t)&_ex06Handler); //InvalidOpCode

	setup_IDT_entry (0x20, (uint64_t)&_irq00Handler);
	timer_Restart(); //Inicializa el timer_Tick


	setup_IDT_entry (0x21, (uint64_t)&_irq01Handler);

	setup_IDT_entry (0x28, (uint64_t)&_irq08Handler); //RTC int

	setup_IDT_entry (0x80, (uint64_t)&_int80handler);

	//Habilita 1 y 2 (Timer_Tick y teclado)
	_picMasterMask(0xFC);
	_picSlaveMask(0x00);

	//Configuro frecuencia PIC
	int hz = PIT_FREQUENCY;
	int divisor = 1193180 / hz;       
    outportb(PIT_CMDREG , 0x36);             
    outportb(PIT_CHANNEL0, divisor & 0xFF);   
    outportb(PIT_CHANNEL0, divisor >> 8);

	/*
	//Habilita 1 y 2 (Timer_Tick y teclado) y picSlave (3?)
	_picMasterMask(0xF8);
	//Habilita 1 (RTC interrupt)
	_picSlaveMask(0xFE);
	
	//Configuro
	outportb(0x70, 0x8B);		// select register B, and disable NMI
	char prev=inportb(0x71);	// read the current value of register B
	outportb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
	outportb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B*/

	_sti();

}

void pruebaRTC() {
	_cli();
	printf("\nHOLA\n");
	outportb(0x70, 0x8B);		// select register B, and disable NMI
	char prev=inportb(0x71);	// read the current value of register B
	outportb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
	outportb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	_sti();
}

void irqDispatcher(int n) {
	switch(n) {
		case 0: //Timer_Tick
			timer_Tick();
			break;
		case 1: //Keyboard
			kb_fetch();
			break;
		case 8: //RTC int
			pruebaRTC();
			break;
	}
}


void dumpData(char * msg, uint64_t * RIP, uint64_t * RSP) {

	uint64_t * oldRSP = RIP + 8; //Apunta al RAX pusheado por el pushState

	printf("Origen del error: %s\n\n", msg);

	printf("Direccion del instruction pointer: %X\n", *RIP);
	printf("Direccion del stack pointer: %X\n", RSP);
	printString("Registros:\n");
	printf("RDI: %X, RSI: %X, RAX: %X, RBX: %X, RCX: %X, RDX: %X\n", oldRSP[8*5], oldRSP[8*6], oldRSP[8*0], oldRSP[8*1], oldRSP[8*2], oldRSP[8*3]);
	printf("R8: %X, R9: %X, R10: %X, R11: %X, R12: %X, R13: %X, R14: %X, R15: %X\n", oldRSP[8*7], oldRSP[8*8], oldRSP[8*9], oldRSP[8*10], oldRSP[8*11], oldRSP[8*12], oldRSP[8*13], oldRSP[8*14]);

}

void exDispatcher(int n, uint64_t * RIP, uint64_t * RSP, uint64_t r) {
  	beep(60, 3);

	int fColor = getFontColor();
	int bColor = getBackgroundColor();

	setFontColor(0x000000);
	setBackgroundColor(0xDC143C);

	extern uint64_t * instructionPointerBackup;
	extern void * stackPointerBackup;

	uint64_t RIP_Back = (uint64_t)&RIP;
	uint64_t RSP_Back = (uint64_t)&RSP;

	switch(n) {
		case 0: //Division by 0
			//*RIP = (uint64_t)instructionPointerBackup;
			//*RSP = (uint64_t)stackPointerBackup;
			dumpData("EXCEPCION (DIVISION POR CERO)", (uint64_t *)RIP_Back, (uint64_t *)RSP_Back);
			printf("\n\n");
			break;

		case 6: //InvalidOpCode
			kernelPanic("EXCEPCION (CODIGO DE OPERACION INVALIDO)", RIP, RSP); //Lo dejo porque la pantallita quedaba divertida
			//*RIP = (uint64_t)instructionPointerBackup;
			//*RSP = (uint64_t)stackPointerBackup;
			dumpData("EXCEPCION (CODIGO DE OPERACION INVALIDO)", (uint64_t *)RIP_Back, (uint64_t *)RSP_Back);
			printf("\n\n");
			break;
	}

	setFontColor(fColor);
	setBackgroundColor(bColor);

	killProcess(getCurrentPID(), -1);
	_force_scheduler();

}
