#include <stdint.h>
#include <interruptions/defs.h>
#include <interruptions/intHandlers.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>
#include <include/global_variables.h>

/* Assembly functions */
void _sti();
void _cli();
void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);

/* Handlers (Assembly) */
void _ex00Handler();
void _ex06Handler();

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

void writeIDT() {
	_cli();	

	setup_IDT_entry (0x0, (uint64_t)&_ex00Handler); //DivByZero
	setup_IDT_entry (0x6, (uint64_t)&_ex06Handler); //InvalidOpCode

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

//Getters de los registros
uint64_t _rax();
uint64_t _rbx();
uint64_t _rcx();
uint64_t _rdx();
uint64_t _rbp();
uint64_t _rsp();
uint64_t _rdi();
uint64_t _rsi();
uint64_t _r8();
uint64_t _r9();
uint64_t _r10();
uint64_t _r11();
uint64_t _r12();
uint64_t _r13();
uint64_t _r14();
uint64_t _r15();

void _rsp_set(uint64_t address);
void _rdi_set(uint64_t address);

void _popState();
void _pushState();

void dumpData(char * msg, uint64_t * RIP, uint64_t * RSP) {

	uint64_t * oldRSP = RIP + 8; //Apunta al RAX pusheado por el pushState

	printf("Origen del error: %s\n\n", msg);

	printf("Direccion del instruction pointer: %X\n", RIP);
	printf("Direccion del stack pointer: %X\n", RSP);
	printString("Registros:\n");
	printf("RDI: %X, RSI: %X, RAX: %X, RBX: %X, RCX: %X, RDX: %X\n", oldRSP[8*5], oldRSP[8*6], oldRSP[8*0], oldRSP[8*1], oldRSP[8*2], oldRSP[8*3]);
	printf("R8: %X, R9: %X, R10: %X, R11: %X, R12: %X, R13: %X, R14: %X, R15: %X\n", oldRSP[8*7], oldRSP[8*8], oldRSP[8*9], oldRSP[8*10], oldRSP[8*11], oldRSP[8*12], oldRSP[8*13], oldRSP[8*14]);

}

unsigned char poolKey();
unsigned char readKey();

void awaitKeyPress() {
	incLine(2);
	printf("Presione una tecla para continuar.");

	printf("\n\n");

	unsigned char c;
	c = poolKey();
	printChar(c);

}

void exDispatcher(int n, uint64_t * RIP, uint64_t * RSP, uint64_t r) {
	
	int fColor = getFontColor();
	int bColor = getBackgroundColor();

	setFontColor(0x000000);
	setBackgroundColor(0xDC143C);

	extern uint64_t * instructionPointerBackup;
	extern void * stackPointerBackup;

	uint64_t RIP_Back = &RIP;
	uint64_t RSP_Back = &RSP;

	switch(n) {
		case 0: //Division by 0
			*RIP = instructionPointerBackup;
			*RSP = stackPointerBackup;
			dumpData("EXCEPCION (DIVISION POR CERO)", RIP_Back, RSP_Back);
			printf("\n\n");
			break;

		case 6: //InvalidOpCode
			//kernelPanic("EXCEPCION (CODIGO DE OPERACION INVALIDO)", RIP, RSP); //Lo dejo porque la pantallita quedaba divertida
			*RIP = instructionPointerBackup;
			*RSP = stackPointerBackup;
			dumpData("EXCEPCION (CODIGO DE OPERACION INVALIDO)", RIP_Back, RSP_Back);
			printf("\n\n");
			break;
	}

	setFontColor(fColor);
	setBackgroundColor(bColor);

	//_rdi_set(stackPointerBackup);

}