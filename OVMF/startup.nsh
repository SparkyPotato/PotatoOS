@echo -off
mode 80 25

cls
if exist .\EFI\BOOT\BootX64.efi then
 .\EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs0:\EFI\BOOT\BootX64.efi then
 fs0:
 echo Found bootloader on fs0:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs1:\EFI\BOOT\BootX64.efi then
 fs1:
 echo Found bootloader on fs1:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs2:\EFI\BOOT\BootX64.efi then
 fs2:
 echo Found bootloader on fs2:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs3:\EFI\BOOT\BootX64.efi then
 fs3:
 echo Found bootloader on fs3:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs4:\EFI\BOOT\BootX64.efi then
 fs4:
 echo Found bootloader on fs4:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs5:\EFI\BOOT\BootX64.efi then
 fs5:
 echo Found bootloader on fs5:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs6:\EFI\BOOT\BootX64.efi then
 fs6:
 echo Found bootloader on fs6:
 EFI\BOOT\BootX64.efi
 goto END
endif

if exist fs7:\EFI\BOOT\BootX64.efi then
 fs7:
 echo Found bootloader on fs7:
 EFI\BOOT\BootX64.efi
 goto END
endif

 echo "Unable to find bootloader".
 
:END
