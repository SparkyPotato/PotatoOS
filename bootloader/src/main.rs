#![no_main]
#![no_std]
#![feature(abi_efiapi)]

use core::{cell::UnsafeCell, fmt::Write, panic::PanicInfo};

use common::POTATO_OS_PARTITION_TYPE_UUID_BYTES;
use uefi::{
	prelude::*,
	proto::media::{block::BlockIO, partition::PartitionInfo},
	table::{boot::SearchType, runtime::ResetType},
};

#[entry]
fn main(handle: Handle, mut table: SystemTable<Boot>) -> Status {
	enter(&mut table);

	{
		let handles = table
			.boot_services()
			.locate_handle_buffer(SearchType::from_proto::<PartitionInfo>())
			.unwrap();

		for &handle in handles.handles() {
			let partition_info = table
				.boot_services()
				.open_protocol_exclusive::<PartitionInfo>(handle)
				.unwrap();

			if let Some(part) = partition_info
				.gpt_partition_entry()
				.filter(|x| x.partition_type_guid.0.to_bytes() == POTATO_OS_PARTITION_TYPE_UUID_BYTES)
			{
				println!("Found kernel partition");

				if let Ok(block_io) = table.boot_services().open_protocol_exclusive::<BlockIO>(handle) {
					let block_count = part.num_blocks().unwrap();
					let start_block = part.starting_lba;
					let kernel_size = block_size as u64 * block_count;

					println!("Got Block IO");
					let media = block_io.media();
					let block_size = media.block_size();

					println!("Kernel size: {}", kernel_size);
				}

				break;
			}
		}
	}

	println!("Error loading kernel");
	table.boot_services().stall(10000000);
	table
		.runtime_services()
		.reset(ResetType::Shutdown, Status::ABORTED, None)
}

fn enter(table: &mut SystemTable<Boot>) {
	unsafe {
		*TABLE.0.get() = Some(table.unsafe_clone());
	}

	table.stdout().clear().unwrap();
	println!("Bootloader initialized; loading kernel");
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
				.reset(ResetType::Shutdown, Status::ABORTED, None);
		}
	}

	loop {
		core::hint::spin_loop();
	}
}

#[macro_export]
macro_rules! println {
    ($($x:tt)*) => {
		unsafe {
			if let Some(ref mut table) = *TABLE.0.get() {
				writeln!(table.stdout(), $($x)*).unwrap();
			}
		}
	};
}
