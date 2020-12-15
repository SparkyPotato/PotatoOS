BCFLAGS = -ISource/Boot -IDependencies/gnu-efi/inc -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone
BLDFLAGS = -subsystem:efi_application -nodefaultlib -dll -entry:_Boot

LDSCRIPT = Kernel.ld
KCFLAGS = -ISource/Kernel -target x86_64-unknown-linux-gnu -fshort-wchar -mno-red-zone
KLDFLAGS = -T $(LDSCRIPT) -static -Bsymbolic -nostdlib

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

BSOURCES := $(call rwildcard, Source/Boot, *.c)
BOBJS = $(BSOURCES:%.c=Intermediate/%.o)

EFISOURCES := $(call rwildcard, Dependencies/gnu-efi, *.c)
EFIOBJS = $(EFISOURCES:%.c=Intermediate/%.o)

KSOURCES := $(call rwildcard, Source/Kernel, *.cpp)
KOBJS = $(KSOURCES:%.cpp=Intermediate/%.o)

bootloader : $(EFIOBJS) $(BOBJS)
	@echo Linking bootloader
	@mkdir -p $(shell dirname Binaries/Boot/Boot.efi)
	@lld-link $(BLDFLAGS) $(BOBJS) $(EFIOBJS) -out:"Binaries/Boot/Boot.efi"

kernel : $(KOBJS)
	@echo Linking kernel
	@mkdir -p $(shell dirname Binaries/Kernel/Kernel.elf)
	@ld.lld $(KLDFLAGS) $(KOBJS) -o "Binaries/Kernel/Kernel.elf"

image :
	@echo Generating Image
	@dd if=/dev/zero of=Binaries/PotatoOS.img bs=512 count=93750
	@mformat -i Binaries/PotatoOS.img -f 1440 ::
	@mmd -i Binaries/PotatoOS.img ::/EFI
	@mmd -i Binaries/PotatoOS.img ::/EFI/BOOT
	@mcopy -i Binaries/PotatoOS.img Binaries/Boot/Boot.efi ::/EFI/BOOT
	@mcopy -i Binaries/PotatoOS.img OVMF/startup.nsh ::
	@mmd -i Binaries/PotatoOS.img ::/Kernel
	@mcopy -i Binaries/PotatoOS.img Binaries/Kernel/Kernel.elf ::/Kernel
	@mmd -i Binaries/PotatoOS.img ::/Content
	@mcopy -i Binaries/PotatoOS.img Content/Font.psf ::/Content

run :
	@qemu-system-x86_64 -drive file=Binaries/PotatoOS.img,format=raw -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMF/OVMFCode.fd",readonly=on -net none

Intermediate/Source/Boot/%.o : Source/Boot/%.c
	@echo $<
	@mkdir -p $(shell dirname $@)
	@clang $(BCFLAGS) -o $@ -c $<

Intermediate/Dependencies/gnu-efi/%.o : Dependencies/gnu-efi/%.c
	@echo $<
	@mkdir -p $(shell dirname $@)
	@clang $(BCFLAGS) -o $@ -c $<
	
Intermediate/Source/Kernel/%.o : Source/Kernel/%.cpp
	@echo $<
	@mkdir -p $(shell dirname $@)
	@clang++ $(KCFLAGS) -o $@ -c $<
