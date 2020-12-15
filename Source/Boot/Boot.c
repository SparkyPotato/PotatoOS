#include "efi.h"
#include "efilib.h"

#include "Elf.h"
#include "File.h"
#include "Graphics.h"

int _fltused = 0;

EFI_STATUS _Boot(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	systemTable->ConOut->ClearScreen(systemTable->ConOut);
	InitializeLib(imageHandle, systemTable);
	
	EFI_FILE* dir = LoadFile(NULL, L"Kernel", imageHandle, systemTable);
	EFI_FILE* kernel = LoadFile(dir, L"Kernel.elf", imageHandle, systemTable);
	if (!kernel) { Print(L"Failed to load kernel!\r\n"); return 1; }
	
	Elf64_Ehdr elfHeader;
	UINTN size = sizeof(elfHeader);
	kernel->Read(kernel, &size, (void*)&elfHeader);
	if (
		memcmp(&elfHeader.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		elfHeader.e_ident[EI_CLASS] != ELFCLASS64 ||
		elfHeader.e_ident[EI_DATA] != ELFDATA2LSB ||
		elfHeader.e_type != ET_EXEC ||
		elfHeader.e_machine != EM_X86_64 ||
		elfHeader.e_version != EV_CURRENT
	)
	{ Print(L"Failed to verify kernel!\r\n"); return 1; }

	Elf64_Phdr* pHeader;
	kernel->SetPosition(kernel, elfHeader.e_phoff);
	size = elfHeader.e_phnum * elfHeader.e_phentsize;
	systemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&pHeader);
	kernel->Read(kernel, &size, pHeader);

	for (Elf64_Half i = 0; i < elfHeader.e_phnum; i++)
	{
		switch (pHeader[i].p_type)
		{
		case PT_LOAD:
			size_t pages = (pHeader[i].p_memsz + 0x1000 - 1) / 0x1000;
			Elf64_Addr segment = pHeader[i].p_paddr;
			systemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
			kernel->SetPosition(kernel, pHeader[i].p_offset);
			size = pHeader[i].p_filesz;
			kernel->Read(kernel, &size, (void*)segment);
			break;
		}
	}

	void (*kernelMain)(void*) = (void (*)(void*))elfHeader.e_entry;
	Framebuffer buffer = InitializeGraphics();
	
	dir = LoadFile(NULL, L"Content", imageHandle, systemTable);
	PSF1Font* font = LoadPSF1Font(dir, L"Font.psf", imageHandle, systemTable);
	if (!font) { Print(L"Failed to load font!\r\n"); return 1; }

	struct
	{
		Framebuffer* Buffer;
		PSF1Font* Font;
	} kernelArgs;
	kernelArgs.Buffer = &buffer;
	kernelArgs.Font = font;
	
	// Since the MS ABI is used, the first (and only!) argument
	// is passed on the RCX register.
	kernelMain(&kernelArgs);
	
	return EFI_SUCCESS;
}
