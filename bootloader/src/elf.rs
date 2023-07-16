use common::KernelEntry;
use elf::{abi::PT_LOAD, endian::NativeEndian, ElfBytes};
use uefi::table::{
	boot::{AllocateType, MemoryType},
	Boot,
	SystemTable,
};

use crate::{load::KernelElf, println};

pub struct KernelMemory {
	pub base_addr: u64,
	pub entry: u64,
	pub pages: u64,
}

pub fn parse_kernel(table: &mut SystemTable<Boot>, kernel: KernelElf) -> KernelMemory {
	println!("Parsing kernel");

	let elf = ElfBytes::<NativeEndian>::minimal_parse(kernel.data).expect("Failed to parse kernel");

	// The kernel is made up of only one segment.
	let phdr = elf.segments().unwrap().iter().next().expect("Kernel has no segments");

	let size = phdr.p_memsz;
	println!("Kernel runtime size: {} bytes", size);

	let page_count = (size + 4095) / 4096;
	let base_addr = table
		.boot_services()
		.allocate_pages(AllocateType::AnyPages, MemoryType::LOADER_DATA, page_count as _)
		.expect("Failed to allocate kernel memory");
	let src = kernel.data.as_ptr();

	unsafe {
		core::ptr::write_bytes(base_addr as *mut u8, 0, size as _);
		core::ptr::copy_nonoverlapping(src, base_addr as _, phdr.p_filesz as _);

		println!("Kernel patched at {:#x}", base_addr);

		KernelMemory {
			base_addr,
			entry: elf.ehdr.e_entry,
			pages: page_count,
		}
	}
}
