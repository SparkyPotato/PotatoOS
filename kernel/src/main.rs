#![no_std]
#![no_main]

fn _start() {}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! { loop {} }
