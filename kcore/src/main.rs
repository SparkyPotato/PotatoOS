#![no_std]
#![no_main]

use common::KernelEntry;

#[no_mangle]
extern "sysv64" fn kinit() -> ! { loop {} }

const _CONFIRM_ENTRY_TYPE: KernelEntry = kinit;

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! { loop {} }
