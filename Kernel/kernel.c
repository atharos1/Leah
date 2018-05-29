#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <drivers/console.h>
#include <drivers/rtc.h>
#include <interruptions/idt.h>
#include <drivers/video_vm.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

void pruebaSysCallWrite();
void _halt();

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

int main()
{

	writeIDT();

	printString("[Kernel Main]");
	incLine(1);
	printString("  Sample code module at 0x");
	printBase((uint64_t)sampleCodeModuleAddress, 16);
	incLine(1);
	incLine(1);	

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

	//printf("Fecha y hora del sistema: %X/%X/%X %X:%X:%X\n\n", RTC(MONTH_DAY), RTC(MONTH), RTC(YEAR), RTC(HOURS), RTC(MINUTES), RTC(SECONDS));

	//ncPrintHex( (*sec >> 8) );

	//ncPrintDec(readKey());

	return 0;
}
