#![no_std]
#![no_main]

use common::{KernelEntry, KernelInfo};

#[no_mangle]
extern "sysv64" fn kinit(info: *mut KernelInfo) -> ! {
	let info = unsafe { &mut *info };

	loop {}
}

const _CONFIRM_ENTRY_TYPE: KernelEntry = kinit;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! { loop {} }
