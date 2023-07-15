#![no_std]

use uefi::table::{Boot, SystemTable};

pub const POTATO_OS_PARTITION_TYPE_UUID_STR: &str = "64919c40-45f4-49f1-b911-a5c317b140eb";
pub const POTATO_OS_PARTITION_TYPE_UUID_BYTES: [u8; 16] =
	[64, 156, 145, 100, 244, 69, 241, 73, 185, 17, 165, 195, 23, 177, 64, 235];

pub const KERNEL_MEMORY_TYPE: u32 = 0x81234567;

pub type KernelEntry = extern "sysv64" fn(KernelInfo) -> !;

#[repr(C)]
pub struct KernelInfo {
	pub table: SystemTable<Boot>,
}
