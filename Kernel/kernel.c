#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <drivers/console.h>
#include <drivers/video_vm.h>
#include <drivers/speaker.h>
#include <interruptions/idt.h>
#include <asm/libasm.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

typedef int (*EntryPoint)();
static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void * const  systemVar = (void *)0x0000000000005A00;
static const uint64_t PageSize = 0x1000;

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int main()
{
	init_VM_Driver();
	setFontSize(1);

	writeIDT();

	printString("[Kernel Main]");
	incLine(1);
	printString("  Sample code module at 0x");
	printBase((uint64_t)sampleCodeModuleAddress, 16);
	incLine(1);
	incLine(1);

	short int * mem_amount = (void *)(systemVar + 132); //EN MB
	printf("\nCantidad de RAM instalada: %dMB\n", *mem_amount);

	//clearScreen();

	extern uint64_t * instructionPointerBackup;
	instructionPointerBackup = sampleCodeModuleAddress;
	extern void * stackPointerBackup;
	stackPointerBackup = _rsp() - 2*8; //Llamada a función pushea ESTADO LOCAL (o algo asi) y dir de retorno?

	//int returnValue = ((EntryPoint)sampleCodeModuleAddress)();

	incLine(1);
	//printf("El programa finalizo con codigo de respuesta: %d\n", returnValue);
	printf("Userlandsize\n");
	printBase(userlandSize, 16);
	incLine(1);

	return 0;
}
