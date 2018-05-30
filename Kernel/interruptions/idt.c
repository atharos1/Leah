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

void kernelPanic() {
	setFontColor(0x000000);
	setBackgroundColor(0xDC143C);

	clearScreen();

	setFontSize(15);

	printf(":D");

	setFontSize(8);
	incLine(2);

	printf("Se fue todo\na la mierda.\n");
	setFontSize(4);
	incLine(4);
	printf("Nada, eso.\n");

	setFontSize(1);
	incLine(30);
	dumpData();
	
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

void dumpData() {

	uint64_t raxBackup = _rax();

	printf("\nDireccion del stack pointer: %X\n", _rsp());
	printf("Direccion del base pointer: %X\n", _rbp());
	printString("\nRegistros:\n");
	printf("RDI: %X, RSI: %X, RAX: %X, RBX: %X, RCX: %X, RDX: %X\n", _rdi(), _rsi(), raxBackup, _rbx(), _rcx(), _rdx());
	printf("R8: %X, R9: %X, R10: %X, R11: %X, R12: %X, R13: %X, R14: %X, R15: %X\n", _r8(), _r9(), _r10(), _r11(), _r12(), _r13(), _r14(), _r15());

	/*while( c == EOF ) {
		_halt();
	}

	clearScreen();*/
	//_rsp_set(stackPointerBackup);

	//((EntryPoint)sampleCodeModuleAddress)();
}

void exDispatcher(int n) {

	kernelPanic();
	
	int fColor = getFontColor();
	int bColor = getBackgroundColor();

	setFontColor(0x000000);
	setBackgroundColor(0xDC143C);

	switch(n) {
		case 0: //Division by 0
			printf("\n\nEXCEPCION: DIVISION POR CERO\n\n");
			break;

		case 6: //InvalidOpCode
			printf("\n\nEXCEPTION: CODIGO DE OPERACION INVALIDO\n\n");
	}

	dumpData();

	setFontColor(fColor);
	setBackgroundColor(bColor);

}