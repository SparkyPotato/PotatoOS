#pragma once

#include "EFI.h"
#include "Protocol/SimpleTextInput.h"
#include "Protocol/SimpleTextOutput.h"
#include "RuntimeServices.h"
#include "BootServices.h"
#include "ConfigurationTable.h"

typedef struct EFI_SYSTEM_TABLE
{
	EFI_TABLE_HEADER Hdr;
	CHAR16* FirmwareVendor;
	UINT32 FirmwareRevision;
	EFI_HANDLE ConsoleInHandle;
	EFI_SIMPLE_TEXT_INPUT_PROTOCOL* ConIn;
	EFI_HANDLE ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
	EFI_HANDLE StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* StdErr;
	EFI_RUNTIME_SERVICES* RuntimeServices;
	EFI_BOOT_SERVICES* BootServices;
	UINTN NumberOfTableEntries;
	EFI_CONFIGURATION_TABLE* ConfigurationTable;
} EFI_SYSTEM_TABLE;
