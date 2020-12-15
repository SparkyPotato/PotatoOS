#include "File.h"

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

EFI_FILE* LoadFile(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
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
