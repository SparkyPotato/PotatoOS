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

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

// Since the kernel is compiled using the SysV ABI,
// the first three arguments are dummies,
// and the fourth argument comes from the RCX register.
extern "C" void _KernelMain(int d0, int d1, int d2, KernelArgs* args)
{
	Memory::Initialize(args->MemoryMap, args->MapSize, args->MapDescriptorSize);
	uint64_t kSize = uint64_t(&_KernelEnd) - uint64_t(&_KernelStart);
	uint64_t kPages = kSize / 4096 + 1;
	Memory::LockPages(&_KernelStart, kPages);

	TextRenderer output(args->Buffer, args->Font);
	output.Clear();
}
