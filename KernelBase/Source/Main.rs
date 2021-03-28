#![allow(non_snake_case)]
#![feature(asm)]
#![no_main]
#![no_std]

mod Debug;

use core::panic::PanicInfo;

use Debug::VGABuffer::{ Color, ColorCode, DebugSetColor };

#[no_mangle]
pub extern fn _start() -> !
{
	DebugPrintLn!("Hello, World!");
	DebugSetColor(ColorCode::New(Color::Cyan, Color::Black));
	DebugPrintLn!("Bye, World!");
	
	loop {}
}

#[panic_handler]
fn RustPanicHandler(info: &PanicInfo) -> !
{
	DebugPrintLn!("{}", info);
	loop {}
}

