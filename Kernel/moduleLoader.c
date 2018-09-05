#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <drivers/console.h>

static uint32_t loadModule(uint8_t ** module, void * targetModuleAddress);
static uint32_t readUint32(uint8_t ** address);

uint32_t * userlandSize = 600000;

void loadModules(void * payloadStart, void ** targetModuleAddress)
{
	int i;
	uint8_t * currentModule = (uint8_t*)payloadStart;
	uint32_t moduleCount = readUint32(&currentModule);

	for (i = 0; i < moduleCount; i++)
		(*userlandSize) = loadModule(&currentModule, targetModuleAddress[i]);
}

static uint32_t loadModule(uint8_t ** module, void * targetModuleAddress)
{
	uint32_t moduleSize = readUint32(module);

	memcpy(targetModuleAddress, *module, moduleSize);
	*module += moduleSize;

	return moduleSize;
}

static uint32_t readUint32(uint8_t ** address)
{
	uint32_t result = *(uint32_t*)(*address);
	*address += sizeof(uint32_t);
	return result;
}
