use uefi::{
	proto::console::gop::{self, GraphicsOutput},
	table::{boot::SearchType, Boot, SystemTable},
};

pub enum PixelFormat {
	Rgb,
	Bgr,
}

pub struct Framebuffer {
	pub buffer: *mut u8,
	pub width: u64,
	pub height: u64,
	pub stride: u64,
	pub format: PixelFormat,
}

pub fn get_framebuffer(table: &mut SystemTable<Boot>) -> Framebuffer {
	let handles = table
		.boot_services()
		.locate_handle_buffer(SearchType::from_proto::<GraphicsOutput>())
		.expect("Failed to get framebuffer protocol");
	let handle = handles.into_iter().next().expect("No framebuffer found");
	let gop = table
		.boot_services()
		.open_protocol_exclusive::<GraphicsOutput>(handle)
		.expect("Failed to open framebuffer protocol");

	let buffer = gop.frame_buffer();
	let mode = gop.current_mode_info();
	let stride = mode.stride();
	let (width, height) = mode.resolution();
	let format = match mode.pixel_format() {
		gop::PixelFormat::Rgb => PixelFormat::Rgb,
		gop::PixelFormat::Bgr => PixelFormat::Bgr,
		_ => panic!("Unsupported pixel format"),
	};

	Framebuffer {
		buffer,
		width,
		height,
		stride,
		format,
	}
}
