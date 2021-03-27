import argparse
import os
from pathlib import Path
import shutil
import subprocess

GComponents = ["bootloader", "kernelbase", "image"]

def Run(mode):
	os.chdir(f"target/{mode}")
	subprocess.run(["qemu-system-x86_64", "-drive", "file=PotatoOS.iso,format=raw",  "-m", "256M", "-cpu", "qemu64", "-drive", "if=pflash,format=raw,unit=0,file=../../OVMF/OVMFCode.fd,readonly=on", "-net", "none"])
	os.chdir("../..")

def BuildImage(mode):
	if os.name != "posix":
		print("error: cannot generate OS image on the current OS, only POSIX is supported. If you are on Windows, you can use WSL")
		exit(1)
	os.chdir(f"target/{mode}")
	subprocess.run(["dd", "if=/dev/zero", "of=PotatoOS.iso", "bs=512", "count=93750"])
	subprocess.run(["mformat", "-i", "PotatoOS.iso", "-f", "1440", "::"])
	subprocess.run(["mmd", "-i", "PotatoOS.iso", "::/EFI"])
	subprocess.run(["mmd", "-i", "PotatoOS.iso", "::/EFI/BOOT"])
	subprocess.run(["mcopy", "-i", "PotatoOS.iso", f"../x86_64-unknown-uefi/{mode}/BootX64.efi", "::/EFI/BOOT"])
	subprocess.run(["mmd", "-i", "PotatoOS.iso", "::/KernelBase"])
	subprocess.run(["mcopy", "-i", "PotatoOS.iso", f"../x86_64/{mode}/KernelBase", "::/KernelBase"])

	os.chdir("../..")

def BuildModule(module, mode):
	os.chdir(module)
	args = ["cargo", "build"]
	if mode == "release": args.append("--release")
	subprocess.run(args)
	os.chdir("..")

def VerifyComponents(components):
	for i, matches in enumerate(map(lambda component: component in GComponents, components)):
		if not matches:
			print(f"error: '{components[i]}' is not a valid component")
			exit(1)
		
def Main():
	parser = argparse.ArgumentParser(description="Python build script for PotatoOS")
	parser.add_argument("components", default=GComponents, type=str, nargs="*", help="The components of PotatoOS to build")
	parser.add_argument("--mode", default="debug", type=str, help="mode to build PotatoOS in (debug or release)")
	parser.add_argument("--run", action="store_true", help="Run the OS in QEMU", default=False)

	args = parser.parse_args()
	VerifyComponents(args.components)
	lowered = args.mode.lower()
	if lowered != "debug" and lowered != "release":
		print(f"error: '{args.mode}' is not a valid build mode")
		exit(1)

	workspaceDirectory = Path(__file__).resolve().parent
	os.chdir(workspaceDirectory)

	if "bootloader" in args.components:
		BuildModule("Bootloader", lowered)
	if "kernelbase" in args.components:
		BuildModule("KernelBase", lowered)
	if "image" in args.components:
		BuildImage(lowered)
	if args.run:
		Run(lowered)

if __name__ == "__main__":
	Main()
