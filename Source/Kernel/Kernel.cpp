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

	uint64_t entries = args->MapSize / args->MapDescriptorSize;
	output.Print("Entries: ");
	output.Print(::Format(entries));
	output.Print("\r\n");
	for (uint64_t i = 0; i < entries; i++)
	{
		MemoryDescriptor* desc = reinterpret_cast<MemoryDescriptor*>(args->MemoryMap + args->MapDescriptorSize * i);

		output.Print(MemoryMapTypes[desc->Type]);
		output.Print(": ");
		output.SetColor(0xffff00ff);
		output.Print(Format(desc->PageCount * 4));
		output.Print(" KB\r\n");
		output.SetColor(0xffffffff);
	}

	output.Print("Done.\r\n");
}
