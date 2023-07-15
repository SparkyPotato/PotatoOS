#![no_std]
#![no_main]

use core::fmt::Write;

use common::{KernelEntry, KernelInfo};

extern "sysv64" fn _start(mut info: KernelInfo) -> ! {
	write!(info.table.stdout(), "Hello, world!").unwrap();
	loop {}
}

const _CONFIRM_ENTRY_TYPE: KernelEntry = _start;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! { loop {} }
