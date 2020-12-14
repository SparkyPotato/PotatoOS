CFLAGS = -ISource -target x86_64-pc-win32-coff -fno-stack-protector -fshort-wchar -mno-red-zone
LLDFLAGS = -subsystem:efi_application -nodefaultlib -dll -entry:Main

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard, Source, *.c)
OBJS = $(SOURCES:%.c=%.o)

build: $(OBJS)
	@echo Linking
	@mkdir -p $(shell dirname  Binaries/Boot.efi)
	@cd Intermediate; lld-link $(LLDFLAGS) $(OBJS) -out:../Binaries/Boot.efi

image: build
	@echo Generating Image
	@dd if=/dev/zero of=Binaries/PotatoOS.img bs=512 count=93750
	@mformat -i Binaries/PotatoOS.img -f 1440 ::
	@mmd -i Binaries/PotatoOS.img ::/EFI
	@mmd -i Binaries/PotatoOS.img ::/EFI/BOOT
	@mcopy -i Binaries/PotatoOS.img Binaries/Boot.efi ::/EFI/BOOT
	@mcopy -i Binaries/PotatoOS.img OVMF/startup.nsh ::

run: image
	qemu-system-x86_64 -drive file=Binaries/PotatoOS.img -m 256M -cpu qemu64 \
	-drive if=pflash,format=raw,unit=0,file="OVMF/OVMFCode.fd",readonly=on \
	-drive if=pflash,format=raw,unit=1,file="OVMF/OVMFVars.fd" -net none

%.o: %.c
	@echo $<
	@mkdir -p $(shell dirname  Intermediate/$@)
	@clang $(CFLAGS) -o Intermediate/$@ -c $<
