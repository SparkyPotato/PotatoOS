#![no_main]
#![no_std]

use core::{cell::UnsafeCell, fmt::Write, panic::PanicInfo};

use common::KernelInfo;
use elf::parse_kernel;
use uefi::{prelude::*, table::runtime::ResetType};

use crate::load::load_kernel;

mod elf;
mod load;

#[cfg(not(target_arch = "x86_64"))]
compile_error!("Only x86_64 is supported");

#[entry]
fn main(handle: Handle, mut table: SystemTable<Boot>) -> Status {
	enter(&mut table);

	let kernel = load_kernel(&mut table);
	let kernel = parse_kernel(&mut table, kernel);

	println!("Starting kernel");
	table.stdout().clear().unwrap();
	let (table, map) = table.exit_boot_services();

	let mut info = KernelInfo { table, map };
	(kernel.entry)(&mut info)
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
		if let Some(ref mut table) = *TABLE.0.get() {
			write!(table.stdout(), "{}", info).unwrap();
			table.boot_services().stall(10000000);
			table
				.runtime_services()
				.reset(ResetType::SHUTDOWN, Status::ABORTED, None);
		}
	}

	loop {
		core::hint::spin_loop();
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
