use core::fmt;

use lazy_static::lazy_static;
use spin::Mutex;
use volatile::Volatile;

#[macro_export]
macro_rules! DebugPrint
{
    ($($arg:tt)*) => ($crate::Debug::VGABuffer::_Print(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! DebugPrintLn
{
    () => ($crate::DebugPrint!("\n"));
    ($($arg:tt)*) => ($crate::DebugPrint!("{}\n", format_args!($($arg)*)));
}

pub fn DebugSetColor(color: ColorCode)
{
	WRITER.lock().Color = color;
}

lazy_static! 
{
    pub static ref WRITER: Mutex<Writer> = Mutex::new(Writer 
	{
        Column: 0,
        Color: ColorCode::default(),
        Buffer: unsafe { &mut *(0xb8000 as *mut Buffer) },
    });
}

#[allow(dead_code)]
#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[repr(u8)]
pub enum Color 
{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	Pink = 13,
	Yellow = 14,
	White = 15,
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
#[repr(transparent)]
pub struct ColorCode(u8);

impl ColorCode
{
	pub fn New(foreground: Color, background: Color) -> ColorCode
	{
		ColorCode((background as u8) << 4 | (foreground as u8))
	}
}

impl Default for ColorCode
{
	fn default() -> Self 
	{
		ColorCode::New(Color::White, Color::Black)    
	}
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(C)]
struct ScreenChar 
{
	Char: u8,
	Color: ColorCode,
}

const BUFFER_HEIGHT: usize = 25;
const BUFFER_WIDTH: usize = 80;

#[repr(transparent)]
struct Buffer 
{
	Chars: [[Volatile<ScreenChar>; BUFFER_WIDTH]; BUFFER_HEIGHT],
}

pub struct Writer
{
	Column: usize,
	Color: ColorCode,
	Buffer: &'static mut Buffer
}

impl Writer
{
	pub fn WriteByte(&mut self, byte: u8) {
		match byte 
		{
			b'\n' => self.NewLine(),
			byte => {
				if self.Column >= BUFFER_WIDTH {
					self.NewLine();
				}

				let row = BUFFER_HEIGHT - 1;
				let col = self.Column;

				let Color = self.Color;
				self.Buffer.Chars[row][col].write(ScreenChar 
				{
					Char: byte,
					Color,
				});
				self.Column += 1;
			}
		}
	}

	pub fn WriteString(&mut self, s: &str) {
		for byte in s.bytes() 
		{
			match byte 
			{
				0x20..=0x7e | b'\n' => self.WriteByte(byte),
				_ => self.WriteByte(0xfe),
			}

		}
	}

	fn NewLine(&mut self) 
	{
		for row in 1..BUFFER_HEIGHT 
		{
            for col in 0..BUFFER_WIDTH 
			{
                let character = self.Buffer.Chars[row][col].read();
                self.Buffer.Chars[row - 1][col].write(character);
            }
        }
        self.ClearRow(BUFFER_HEIGHT - 1);
        self.Column = 0;
	}

	fn ClearRow(&mut self, row: usize) 
	{
        let blank = ScreenChar {
            Char: b' ',
            Color: self.Color,
        };
        for col in 0..BUFFER_WIDTH {
            self.Buffer.Chars[row][col].write(blank);
        }
    }
}

impl fmt::Write for Writer
{
	fn write_str(&mut self, s: &str) -> fmt::Result 
	{
		self.WriteString(s);

		Ok(())	
	}
}

#[doc(hidden)]
pub fn _Print(args: fmt::Arguments) 
{
    use core::fmt::Write;
    WRITER.lock().write_fmt(args).unwrap();
}
