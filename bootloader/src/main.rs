#![no_main]
#![no_std]

use core::{cell::UnsafeCell, fmt::Write, panic::PanicInfo};

use common::KernelInfo;
use uefi::{prelude::*, table::runtime::ResetType};

use crate::{elf::parse_kernel, load::load_kernel, stack::allocate_kernel_stack};

mod elf;
mod load;
mod stack;

#[cfg(not(target_arch = "x86_64"))]
compile_error!("Only x86_64 is supported");

// TODO:
// - Init kcore page allocator.
// - Init kcore page table manager.
// - Generate page tables:
//   - Runtime services
//   - Identity map
//   - Map kernel
//   - Map kernel stack
// - Enable paging
// - Set UEFI virtual address map

#[entry]
fn main(handle: Handle, mut table: SystemTable<Boot>) -> Status {
	enter(&mut table);

	let kernel = load_kernel(&mut table);
	let kernel = parse_kernel(&mut table, kernel);
	let stack = allocate_kernel_stack(&mut table);

	let (table, map) = table.exit_boot_services();

	let mut info = KernelInfo {};
	// (kernel.entry)(&mut info)
	loop {}
}

fn enter(table: &mut SystemTable<Boot>) {
	unsafe {
		*TABLE.0.get() = Some(table.unsafe_clone());
	}

	table.stdout().clear().unwrap();
	println!("Bootloader initialized");
}

struct SyncTable(UnsafeCell<Option<SystemTable<Boot>>>);
unsafe impl Sync for SyncTable {}

static TABLE: SyncTable = SyncTable(UnsafeCell::new(None));

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
	unsafe {
		// We can never panic before init.
		let table = (&mut *TABLE.0.get()).as_mut().unwrap_unchecked();
		write!(table.stdout(), "{}", info).unwrap();
		table.boot_services().stall(10000000);
		table
			.runtime_services()
			.reset(ResetType::SHUTDOWN, Status::ABORTED, None)
	}
}

#[macro_export]
macro_rules! println {
    ($($x:tt)*) => {
		#[allow(unused_unsafe)]
		unsafe {
			use core::fmt::Write;
			if let Some(ref mut table) = *crate::TABLE.0.get() {
				writeln!(table.stdout(), $($x)*).unwrap();
			}
		}
	};
}
