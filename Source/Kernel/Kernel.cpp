#include "Core/Output.h"
#include "Text/TextRenderer.h"
#include "Text/Formatter.h"

struct KernelArgs
{
	Framebuffer* Buffer;
	PSF1Font* Font;
};

// Since the kernel is compiled using the SysV ABI,
// the first three arguments are dummies,
// and the fourth argument comes from the RCX register.
extern "C" void _KernelMain(int d0, int d1, int d2, struct KernelArgs* args)
{
	TextRenderer output(args->Buffer, args->Font);

	output.SetCursorPosition({ 10, 10 });
	output.Print(Formatter<int64_t>::Format(-1280));
}
