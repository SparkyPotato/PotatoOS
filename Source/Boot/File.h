#pragma once

#include <stdint.h>

#include "efi.h"
#include "efilib.h"

int memcmp(const void* first, const void* second, size_t size);

EFI_FILE* LoadFile(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable);
