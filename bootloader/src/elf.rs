use common::KernelEntry;
use elf::{endian::NativeEndian, ElfBytes};

use crate::load::LoadedKernel;

pub fn parse_kernel(kernel: LoadedKernel) -> KernelEntry {
	let kernel = ElfBytes::<NativeEndian>::minimal_parse(kernel.data).expect("Failed to parse kernel");
	unsafe { core::mem::transmute(kernel.ehdr.e_entry) }
}
