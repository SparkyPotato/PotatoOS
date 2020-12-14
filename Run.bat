pushd %~dp0

qemu-system-x86_64 -drive file=Binaries/PotatoOS.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMF/OVMFCode.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMF/OVMFVars.fd" -net none

popd
pause
