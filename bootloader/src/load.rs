use common::POTATO_OS_PARTITION_TYPE_UUID_BYTES;
use uefi::{
	proto::media::{block::BlockIO, partition::PartitionInfo},
	table::{
		boot::{MemoryType, SearchType},
		Boot,
		SystemTable,
	},
};

use crate::{println, TABLE};

/// The kernel loaded into memory.
///
/// The kernel *does not* expect to be loaded into any specific location, so we just allocate pages directly from the
/// UEFI firmware.
pub struct KernelElf {
	pub data: &'static [u8],
}

impl Drop for KernelElf {
	fn drop(&mut self) {
		unsafe {
			let table = (*TABLE.0.get()).as_mut().unwrap();
			let _ = table.boot_services().free_pool(self.data.as_ptr() as _);
		}
	}
}

pub fn load_kernel(table: &mut SystemTable<Boot>) -> KernelElf {
	let handles = table
		.boot_services()
		.locate_handle_buffer(SearchType::from_proto::<PartitionInfo>())
		.unwrap();

	for &handle in handles.iter() {
		let partition_info = table
			.boot_services()
			.open_protocol_exclusive::<PartitionInfo>(handle)
			.unwrap();

		if let Some(part) = partition_info
			.gpt_partition_entry()
			.filter(|x| x.partition_type_guid.0.to_bytes() == POTATO_OS_PARTITION_TYPE_UUID_BYTES)
		{
			println!("Found kernel partition");

			let block_io = table
				.boot_services()
				.open_protocol_exclusive::<BlockIO>(handle)
				.expect("Failed to get Block IO");
			let media = block_io.media();
			let media_id = media.media_id();
			let block_count = part.num_blocks().expect("Invalid parition size");
			let block_size = media.block_size();

			let kernel_size = block_size as u64 * block_count;
			println!("Kernel file size: {}", kernel_size);

			let memory = table
				.boot_services()
				.allocate_pool(MemoryType::LOADER_DATA, kernel_size as _)
				.expect("Failed to allocate kernel memory");

			let kernel = unsafe { core::slice::from_raw_parts_mut(memory as *mut u8, kernel_size as usize) };
			block_io
				.read_blocks(media_id, 0, kernel)
				.expect("Failed to read kernel");

			println!("Kernel loaded");

			return KernelElf { data: kernel };
		}
	}

	panic!("Error loading kernel")
}
