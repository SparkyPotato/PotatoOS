#include "EFI/SystemTable.h"

EFI_STATUS Main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
    systemTable->ConOut->OutputString(systemTable->ConOut, L"Hello World!\n");

    return EFI_SUCCESS;
}
