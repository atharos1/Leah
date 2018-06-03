#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <drivers/console.h>
#include <drivers/rtc.h>
#include <drivers/video_vm.h>
#include <drivers/speaker.h>
#include <interruptions/idt.h>
#include <include/global_variables.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

void pruebaSysCallWrite();
void _halt();
uint64_t _rsp();
uint64_t _rip();

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

	printString("[x64BareBones]");
	incLine(1);;

	printString("CPU Vendor:");
	printString(cpuVendor(buffer));
	incLine(1);

	printString("[Loading modules]");
	incLine(1);
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	printString("[Done]");
	incLine(1);
	incLine(1);

	printString("[Initializing kernel's binary]");
	incLine(1);

	clearBSS(&bss, &endOfKernel - &bss);

	printString("  text: 0x");
	printBase((uint64_t)&text, 16);
	incLine(1);
	printString("  rodata: 0x");
	printBase((uint64_t)&rodata, 16);
	incLine(1);
	printString("  data: 0x");
	printBase((uint64_t)&data, 16);
	incLine(1);
	printString("  bss: 0x");
	printBase((uint64_t)&bss, 16);
	incLine(1);

	printString("[Done]");

	incLine(1);
	incLine(1);

	return getStackBase();
}

uint64_t _rsp();


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

	clearScreen();

	extern uint64_t * instructionPointerBackup;
	instructionPointerBackup = sampleCodeModuleAddress;
	extern void * stackPointerBackup;
	stackPointerBackup = _rsp() - 2*8; //Llamada a función pushea ESTADO LOCAL (o algo asi) y dir de retorno?

	setSpeaker();
	int returnValue = ((EntryPoint)sampleCodeModuleAddress)();
	//removeFunctionFromTimer(cursorTick);

	incLine(1);
	printf("El programa finalizo con codigo de respuesta: %d\n", returnValue);

	printString("  Sample data module at 0x");
	printBase((uint64_t)sampleDataModuleAddress, 16);

	incLine(1);
	printString("  Sample data module contents: ");
	printString((char*)sampleDataModuleAddress);
	incLine(1);

	printString("[Finished]");

	return 0;
}
