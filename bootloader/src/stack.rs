use common::KERNEL_STACK_SIZE;
use uefi::table::{Boot, SystemTable};

/// Allocate the kernel stack, growing downwards.
pub fn allocate_kernel_stack(table: &mut SystemTable<Boot>) -> u64 {
	let stack_addr = table
		.boot_services()
		.allocate_pages(
			uefi::table::boot::AllocateType::AnyPages,
			uefi::table::boot::MemoryType::LOADER_DATA,
			(KERNEL_STACK_SIZE / 4096) as usize,
		)
		.expect("Failed to allocate kernel stack");

	unsafe {
		core::ptr::write_bytes(stack_addr as *mut u8, 0, KERNEL_STACK_SIZE as _);
	}

	stack_addr + KERNEL_STACK_SIZE
}
