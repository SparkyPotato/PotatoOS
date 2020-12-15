@echo off
pushd %~dp0

qemu-system-x86_64 -drive file=Binaries/PotatoOS.iso,format=raw -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMF/OVMFCode.fd",readonly=on -net none

popd
