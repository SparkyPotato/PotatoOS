#include "Core/Output.h"
#include "Core/Memory.h"
#include "Text/TextRenderer.h"
#include "Text/Formatter.h"

struct KernelArgs
{
	Framebuffer* Buffer;
	PSF1Font* Font;
	uint8_t* MemoryMap;
	uint64_t MapSize;
	uint64_t MapDescriptorSize;
};

// Since the kernel is compiled using the SysV ABI,
// the first three arguments are dummies,
// and the fourth argument comes from the RCX register.
extern "C" void _KernelMain(int d0, int d1, int d2, struct KernelArgs* args)
{
	TextRenderer output(args->Buffer, args->Font);
	Memory memory(args->MemoryMap, args->MapSize, args->MapDescriptorSize);
}
