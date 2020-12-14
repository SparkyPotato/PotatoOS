#include <stdint.h>

#include "efi.h"
#include "efilib.h"

#include "Boot/Elf.h"
#include "Boot/Graphics.h"

int _fltused = 0;

EFI_FILE* LoadFile(EFI_FILE* directory, CHAR16* path,
	EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
	systemTable->BootServices->HandleProtocol(imageHandle, &LoadedImageProtocol, (void**) &loadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystem;
	systemTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &FileSystemProtocol, (void**) &fileSystem);

	if (directory == NULL)
	{
		fileSystem->OpenVolume(fileSystem, &directory);
	}

	EFI_FILE* loadedFile;
	EFI_STATUS status = directory->Open(directory, &loadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status != EFI_SUCCESS) { return NULL; }

	return loadedFile;
}

int memcmp(const void* first, const void* second, size_t size)
{
	const uint8_t* a = first;
	const uint8_t* b = second;

	for (size_t i = 0; i < size; i++)
	{
		if (a[i] < b[i]) { return -1; }
		else if (a[i] > b[i]) { return 1; }
	}

	return 0;
}

EFI_STATUS _Boot(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	systemTable->ConOut->ClearScreen(systemTable->ConOut);
	InitializeLib(imageHandle, systemTable);
	
	EFI_FILE* kernel = LoadFile(NULL, L"Kernel.elf", imageHandle, systemTable);
	if (!kernel) { Print(L"Failed to load kernel!\r\n"); }
	
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
	{ Print(L"Failed to verify kernel!\r\n"); }
	else { Print(L"Verified kernel.\r\n"); }

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

	int (*kernelMain)(int) = (int (*)(int))elfHeader.e_entry;
	Framebuffer buffer = InitializeGraphics();

	Print(L"Starting kernel.\r\n");
	Print(L"%d\r\n", kernelMain(1));

	return EFI_SUCCESS;
}
