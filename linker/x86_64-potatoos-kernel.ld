/* Tell the linker that we want an x86_64 ELF64 output file. */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(kinit)

KERNEL_OFFSET = 0xffffffff80000000;

PHDRS
{
    boot PT_LOAD FILEHDR PHDRS;
}

SECTIONS
{
    . = 0xffffffff80000000;
    .text . + SIZEOF_HEADERS : AT(ADDR(.text) - KERNEL_OFFSET + SIZEOF_HEADERS) {
        __text_start = .;
        KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*)   /* code */
        . = ALIGN(4096);
        __text_end = .;
    } :boot

    .rodata : AT(ADDR(.rodata) - KERNEL_OFFSET) {
        __rodata_start = .;
        *(.rodata*)
        . = ALIGN(4096);
        __rodata_end = .;
    } :boot

    .data : AT(ADDR(.data) - KERNEL_OFFSET) {
        __data_start = .;
        *(.data*)
        . = ALIGN(4096);
        __data_end = .;
        __bss_start = .;
        *(.bss*)
        . = ALIGN(4096);
        __bss_end = .;
    } :boot

    /DISCARD/ : { *(.eh_frame) *(.comment) }
}
