#include "Kernel/Graphics.h"

struct KernelArgs
{
	Framebuffer* Buffer;
	PSF1Font* Font;
};

// Since the kernel is compiled using the SysV ABI,
// the first three arguments are dummies,
// and the fourth argument comes from the RCX register.
void _KernelMain(int d0, int d1, int d2, struct KernelArgs* args)
{
	TextOutput out = InitializeTextOutput(args->Buffer, args->Font);
	
	Print(&out, "Hello World! \n", 0xffffffff);
	Print(&out, "- said the kernel \n", 0xffffffff);
}
