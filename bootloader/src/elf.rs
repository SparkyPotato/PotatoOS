use common::{KernelEntry, KERNEL_MEMORY_TYPE};
use elf::{abi::PT_LOAD, endian::NativeEndian, ElfBytes};
use uefi::table::{
	boot::{AllocateType, MemoryType},
	Boot,
	SystemTable,
};

use crate::{load::KernelElf, println};

pub struct ReadyKernel {
	pub entry: KernelEntry,
	pub base_addr: u64,
	pub pages: u64,
}

pub fn parse_kernel(table: &mut SystemTable<Boot>, kernel: KernelElf) -> ReadyKernel {
	println!("Parsing kernel");

	let elf = ElfBytes::<NativeEndian>::minimal_parse(kernel.data).expect("Failed to parse kernel");

	let mut vaddr_begin = u64::MAX;
	let mut vaddr_end = 0;
	for seg in elf.segments().unwrap().iter().filter(|x| x.p_type == PT_LOAD) {
		let align = seg.p_align.max(4096);

		let begin = seg.p_vaddr & !(align - 1);
		vaddr_begin = vaddr_begin.min(begin);
		let end = (seg.p_vaddr + seg.p_memsz + align - 1) & !(align - 1);
		vaddr_end = vaddr_end.max(end);
	}

	let size = vaddr_end - vaddr_begin;
	println!("Kernel runtime size: {} bytes", size);

	let page_count = size / 4096;
	let base_addr = table
		.boot_services()
		.allocate_pages(
			AllocateType::AnyPages,
			MemoryType::custom(KERNEL_MEMORY_TYPE),
			page_count as _,
		)
		.expect("Failed to allocate kernel memory");
	let src = kernel.data.as_ptr();

	unsafe {
		core::ptr::write_bytes(base_addr as *mut u8, 0, size as _);

		for seg in elf.segments().unwrap().iter().filter(|x| x.p_type == PT_LOAD) {
			let out_addr = base_addr + seg.p_vaddr - vaddr_begin;
			core::ptr::copy_nonoverlapping(src.add(seg.p_offset as _), out_addr as _, seg.p_filesz as _);
		}

		let entry = base_addr + elf.ehdr.e_entry - vaddr_begin;
		println!("Kernel ready at {:#x}, entry point at {:#x}", base_addr, entry);

		ReadyKernel {
			entry: core::mem::transmute(entry),
			base_addr,
			pages: page_count,
		}
	}
}
