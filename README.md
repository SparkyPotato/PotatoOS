# PotatoOS

An 'operating system' for people who love potatoes.

## Building and Testing

Since operating systems and EFI are slightly more complex than a simple executable, 
we need `cargo-xtask` to make building easy.

Run `cargo xtask build` to build the OS to `target/debug/boot`.
The folder can be put into a FAT32 formatted drive and booted on a real machine or QEMU.
