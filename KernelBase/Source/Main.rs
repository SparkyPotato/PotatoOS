#![allow(non_snake_case)]
#![no_std]
#![no_main]

use core::panic::PanicInfo;

#[no_mangle]
pub extern fn _start() -> !
{
	loop {}
}

#[panic_handler]
fn RustPanicHandler(_info: &PanicInfo) -> !
{
	loop {}
}
