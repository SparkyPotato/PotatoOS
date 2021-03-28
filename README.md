# PotatoOS
A small, experimental, UEFI OS written in Rust.

## Building
### The kernel and bootloader
Prerequisites:  
1. Rust (and Cargo) installed, and in your path.
2. The `rust-src` component installed: `rustup component add rust-src`.
3. The `llvm-tools-preview` component installed: `rustup component add llvm-tools-preview`.
4. Bootimage installed: `cargo install bootimage`.

Building:
Just call `cargo bootimage`.

## Running
Prerequisites:
1. QEMU installed and in your PATH.

Then just call `cargo run`.
