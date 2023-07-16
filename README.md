# PotatoOS

An 'operating system' for people who love potatoes.

## Building and Testing

Since operating systems and EFI are slightly more complex than a simple executable, 
we need `cargo-xtask` to make building easy.

Run `cargo xtask build` to build the OS to `target/debug/Disk.img`. This can be flashed to a USB drive and booted on a real system.

Alternatively, QEMU with OVMF can be used. With QEMU in path and `$OVMF` point to a folder with `OVMF_CODE.fd` and `OVMF_VARS.fd`, `cargo xtask run` will run the OS.
