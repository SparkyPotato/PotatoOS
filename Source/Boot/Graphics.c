#include "Boot/Graphics.h"

#include "efi.h"
#include "efilib.h"

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
