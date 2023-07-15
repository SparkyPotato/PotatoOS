#![no_std]
#![no_main]

use core::fmt::Write;

use common::{KernelEntry, KernelInfo};
use uefi::table::runtime::ResetType;

#[no_mangle]
extern "sysv64" fn _start(info: *mut KernelInfo) -> ! {
	let info = unsafe { &mut *info };

	loop {}
}

const _CONFIRM_ENTRY_TYPE: KernelEntry = _start;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! { loop {} }
