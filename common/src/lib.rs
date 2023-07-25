#![no_std]

pub const POTATO_OS_PARTITION_TYPE_UUID_STR: &str = "64919c40-45f4-49f1-b911-a5c317b140eb";
pub const POTATO_OS_PARTITION_TYPE_UUID_BYTES: [u8; 16] =
	[64, 156, 145, 100, 244, 69, 241, 73, 185, 17, 165, 195, 23, 177, 64, 235];

pub type KernelEntry = extern "sysv64" fn() -> !;

pub const MIN_ADDR: u64 = 0x0000000000000000;
pub const MAX_ADDR: u64 = 0xffffffffffffffff;

pub const USER_MEMORY_END: u64 = 0x00007fffffffffff;
pub const KERNEL_MEMORY_START: u64 = 0xffff800000000000;
pub const KERNEL_OFFSET: u64 = 0xffffffff80000000;
pub const KERNEL_STACK_SIZE: u64 = 0x4000;
pub const KERNEL_STACK_MIN: u64 = KERNEL_OFFSET - KERNEL_STACK_SIZE;
