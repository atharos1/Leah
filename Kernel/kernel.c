#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <memoryManager.h>
#include <drivers/console.h>
#include <drivers/video_vm.h>
#include <drivers/speaker.h>
#include <interruptions/idt.h>
#include <asm/libasm.h>
#include "scheduler.h"

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

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	uint32_t stackSize = 32768; // 32KiB
	return (void *)((uint32_t)getMemory(stackSize) + stackSize);
	// return (void*)(
	// 	(uint64_t)&endOfKernel
	// 	+ PAGE_SIZE * 8				//The size of the stack itself, 32KiB
	// 	- sizeof(uint64_t)			//Begin at the top of the stack
	// );
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

	//////////// INITIALIZATIONS ////////////

void pruebaTask() {
	//return;
	long int i = 0;
	int j = 0;
	int colors[7] = {
		0x4444DD,
		0x11aabb,
		0xaacc22,
		0xd0c310,
		0xff9933,
		0xff4422,
		0x72a4c9
	};
	while(1) {
		if(i % 9999999 == 0) {
			setFontColor( colors[j%7] );
			j++;
			i = 0;
		}
		i++;
	}
}

void end() {
	while(1) {
		printf("\nFIN DEL PROCESO\n");
		_halt();
	}

}

int main()
{
	init_VM_Driver();

	uint32_t * mem_amount = (void *)(systemVar + 132); //En MiB
	uint64_t mem_amount_bytes = (*mem_amount) * (1 << 20); //En bytes
	uint32_t * userlandSize = 600000;
	init_memoryManager((void *)((char *)sampleDataModuleAddress + *userlandSize), mem_amount_bytes);

	scheduler_init();

	setFontSize(1);

	extern uint64_t * instructionPointerBackup;
	instructionPointerBackup = sampleCodeModuleAddress;
	extern void * stackPointerBackup;
	stackPointerBackup = _rsp() - 2*8; //Llamada a función pushea ESTADO LOCAL (o algo asi) y dir de retorno?

	scheduler_newProcess("Terminalator", sampleCodeModuleAddress, 4, 4);

	scheduler_newProcess("Arcoiris", &pruebaTask, 4, 4);
	scheduler_newProcess("BBBBBB", &pruebaTask, 4, 4);
	
	while(1)
		_halt();

	//int returnValue = ((EntryPoint)sampleCodeModuleAddress)();
	//printf("El programa finalizo con codigo de respuesta: %d\n", returnValue);
	// printf("Userlandsize\n");
	// printBase(userlandSize, 16);

	return 0;
}