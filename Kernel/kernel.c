#include <stdint.h>
#include "include/lib.h"
#include "include/moduleLoader.h"
#include "include/memoryManager.h"
#include "drivers/include/console.h"
#include "drivers/include/video_vm.h"
#include "drivers/include/kb_driver.h"
#include "drivers/include/speaker.h"
#include "interruptions/idt.h"
#include "asm/libasm.h"
#include "include/fileSystem.h"
#include "include/sleep.h"
#include "include/scheduler.h"
#include "include/process.h"
#include "include/malloc.h"

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
	return (void*)(
		(uint64_t)&endOfKernel
		+ PAGE_SIZE * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{

	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	//////////// INITIALIZATIONS ////////////
	init_VM_Driver();

	uint32_t * mem_amount = (void *)(systemVar + 132); //En MiB
	uint64_t mem_amount_bytes = (*mem_amount) * (1 << 20); //En bytes
	uint32_t * userlandSize = (uint32_t *)600000;
	init_memoryManager((void *)((char *)sampleDataModuleAddress + *userlandSize), mem_amount_bytes);
	init_malloc(10*1024*1024);
	init_fileSystem();
	init_kb();

	scheduler_init();

	return getStackBase();
}



int pruebaTask() {
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
		//printf("%d", j);
		j++;
		setFontColor( colors[j%7] );
		sleepCurrentThread(1000);
	}
	return 0;
}


void initThread() {
	scheduler_dequeue_current();

	while(1) {
		if(aliveProcessCount() == 1) {
			createProcess("Terminalator", sampleCodeModuleAddress, (char*[]){NULL},  4, 4);
			giveForeground(1);
		}

		_force_scheduler();
	}
}

int main()
{

  writeIDT();

	setFontSize(1);

	extern uint64_t * instructionPointerBackup;
	instructionPointerBackup = sampleCodeModuleAddress;
	extern void * stackPointerBackup;
	stackPointerBackup = _rsp() - 2*8; //Llamada a función pushea ESTADO LOCAL (o algo asi) y dir de retorno?

	createProcess("Init", &initThread, (char*[]){NULL}, 1, 0);

	_force_scheduler();

	//int returnValue = ((EntryPoint)sampleCodeModuleAddress)();
	//printf("El programa finalizo con codigo de respuesta: %d\n", returnValue);
	// printf("Userlandsize\n");
	// printBase(userlandSize, 16);

	return 0;
}
