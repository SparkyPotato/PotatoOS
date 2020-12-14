#include "EFI/SystemTable.h"
#include "EFI/Protocol/LoadedImage.h"
#include "EFI/Protocol/SimpleFileSystem.h"

EFI_FILE_PROTOCOL* LoadFile(EFI_FILE_PROTOCOL* directory, CHAR16* path,
	EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
	systemTable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGui, (void**)&imageHandle);
	
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fileSystem;
	systemTable->BootServices->HandleProtocol(imageHandle, &gEfiSimpleFileSystemProtocolGui, (void**)&fileSystem);
	
	if (directory == NULL)
	{
		fileSystem->OpenVolume(fileSystem, &directory);
	}
	
	EFI_FILE_PROTOCOL* loadedFile;
	EFI_STATUS status = directory->Open(directory, &loadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (status != EFI_SUCCESS)
	{
		return NULL;
	}
	return loadedFile;
}

EFI_STATUS Main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	systemTable->ConOut->ClearScreen(systemTable->ConOut);
    systemTable->ConOut->OutputString(systemTable->ConOut, L"Bootloader initialized.\n\r");
	
	if (LoadFile(NULL, L"Kernel.elf", imageHandle, systemTable) == NULL)
	{
		systemTable->ConOut->OutputString(systemTable->ConOut, L"Failed to load kernel.\n\r");
	}
	else
	{
		systemTable->ConOut->OutputString(systemTable->ConOut, L"Loaded kernel.\n\r");
	}
	
    return EFI_SUCCESS;
}
