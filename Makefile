BCFLAGS = -ISource -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone
BLDFLAGS = -subsystem:efi_application -nodefaultlib -dll -entry:Main

LDSCRIPT = ../Kernel.ld
KCFLAGS = -ISource -target x86_64-unknown-linux-gnu -fshort-wchar -mno-red-zone
KLDFLAGS = -T $(LDSCRIPT) -shared -Bsymbolic -nostdlib

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

BSOURCES := $(call rwildcard, Source/Boot, *.c)
BOBJS = $(BSOURCES:%.c=%.o)

KSOURCES := $(call rwildcard, Source/Kernel, *.c)
KOBJS = $(KSOURCES:%.c=%.o)

bootloader: $(BOBJS)
	@echo Linking bootloader
	@mkdir -p $(shell dirname Binaries/Boot/BootX64.efi)
	@cd Intermediate; lld-link $(BLDFLAGS) $(BOBJS) -out:"../Binaries/Boot/BootX64.efi"

kernel: $(KOBJS)
	@echo Linking kernel
	@mkdir -p $(shell dirname Binaries/Kernel/Kernel.elf)
	@cd Intermediate; ld.lld $(KLDFLAGS) $(KOBJS) -o "../Binaries/Kernel/Kernel.elf"

image:
	@echo Generating Image
	@dd if=/dev/zero of=Binaries/PotatoOS.img bs=512 count=93750
	@mformat -i Binaries/PotatoOS.img -f 1440 ::
	@mmd -i Binaries/PotatoOS.img ::/EFI
	@mmd -i Binaries/PotatoOS.img ::/EFI/BOOT
	@mcopy -i Binaries/PotatoOS.img Binaries/Boot/BootX64.efi ::/EFI/BOOT
	@mcopy -i Binaries/PotatoOS.img OVMF/startup.nsh ::
	@mcopy -i Binaries/PotatoOS.img Binaries/Kernel/Kernel.elf ::

run:
	qemu-system-x86_64 -drive file=Binaries/PotatoOS.img -m 256M -cpu qemu64 \
	-drive if=pflash,format=raw,unit=0,file="OVMF/OVMFCode.fd",readonly=on \
	-drive if=pflash,format=raw,unit=1,file="OVMF/OVMFVars.fd" -net none

Source/Boot/%.o: Source/Boot/%.c
	@echo $<
	@mkdir -p $(shell dirname Intermediate/$@)
	@clang $(BCFLAGS) -o Intermediate/$@ -c $<
	
Source/Kernel/%.o: Source/Kernel/%.c
	@echo $<
	@mkdir -p $(shell dirname Intermediate/$@)
	@clang $(KCFLAGS) -o Intermediate/$@ -c $<
