#include "Graphics.h"

#include "efi.h"
#include "efilib.h"

#include "File.h"

Framebuffer InitializeGraphics()
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics;
    EFI_STATUS status = BS->LocateProtocol(&GraphicsOutputProtocol, NULL, (void **)&graphics);
    if (EFI_ERROR(status))
    {
        Print(L"Failed to locate Graphics Output Protocol");
        Framebuffer buffer;
        buffer.BaseAddress = NULL;
        return buffer;
    }

    Framebuffer buffer;

    buffer.BaseAddress = (void*)graphics->Mode->FrameBufferBase;
    buffer.Size = graphics->Mode->FrameBufferSize;
    buffer.Width = graphics->Mode->Info->HorizontalResolution;
    buffer.Height = graphics->Mode->Info->VerticalResolution;
    buffer.Stride = graphics->Mode->Info->PixelsPerScanLine;

    return buffer;
}

PSF1Font* LoadPSF1Font(EFI_FILE* directory, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systemTable)
{
	EFI_FILE* font = LoadFile(directory, path, imageHandle, systemTable);
	if (font == NULL) { return NULL; }
	
	PSF1Header* header;
	systemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1Header), (void**)&header);
	UINTN size = sizeof(PSF1Header);
	font->Read(font, &size, header);
	
	if (header->Magic[0] != PSF1_MAGIC0 || header->Magic[1] != PSF1_MAGIC1) { return NULL; }
	
	size = header->CharSize * 256;
	// 512 glyphs
	if (header->Mode == 1) { size *= 2; }
	
	void* buffer;
	font->SetPosition(font, sizeof(PSF1Header));
	systemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&buffer);
	font->Read(font, &size, buffer);
	
	PSF1Font* pFont;
	systemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1Font), (void**)&pFont);
	pFont->Header = header;
	pFont->Glyphs = buffer;
	
	return pFont;
}
