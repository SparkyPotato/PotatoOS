use core::hint::unreachable_unchecked;

use crate::PhysicalPage;

#[derive(Copy, Clone, Debug)]
pub struct MemoryStats {
	pub total_pages: u64,
	pub reserved_pages: u64,
	pub allocated_pages: u64,
}

pub struct PhysicalPageAllocator {
	bitmap: PhysicalPage,
	stats: MemoryStats,
}

#[cfg(feature = "init")]
impl PhysicalPageAllocator {
	pub fn init(mut map: uefi::table::boot::MemoryMap) -> Self {
		// Paging is not enabled during init.

		map.sort();
		let usable = map.entries().flat_map(move |x| {
			let usable = matches!(
				x.ty,
				uefi::table::boot::MemoryType::BOOT_SERVICES_CODE
					| uefi::table::boot::MemoryType::BOOT_SERVICES_DATA
					| uefi::table::boot::MemoryType::CONVENTIONAL
			);
			(0..x.page_count).map(move |_| usable)
		});

		let page_count: u64 = map.entries().map(|x| x.page_count).sum();
		let req = page_count >> 15;

		// Find `req` consecutive usable pages.
		let mut count = 0;
		let mut start = 0;
		let mut end = 0;
		for (i, usable) in usable.clone().enumerate() {
			let i = i as u64;
			if usable {
				count += 1;
				if count == req {
					end = i + 1;
					break;
				}
			} else {
				count = 0;
				start = i + 1;
			}
		}

		// Allocate the bitmap in our reserved pages.
		let mut reserved_pages = 0;
		let ptr = (start << 12) as *mut u8;
		for (i, mut usable) in usable.enumerate() {
			let i = i as u64;

			// Reserve ourselves as well
			if (start..end).contains(&i) {
				usable = false;
			}

			if usable {
				reserved_pages += 1;
				let byte = i >> 3;
				let bit = i & 7;
				unsafe { *ptr.add(byte as usize) |= (usable as u8) << bit };
			}
		}

		Self {
			bitmap: PhysicalPage(start << 12),
			stats: MemoryStats {
				total_pages: page_count,
				reserved_pages,
				allocated_pages: 0,
			},
		}
	}
}

impl PhysicalPageAllocator {
	pub fn stats(&self) -> MemoryStats { self.stats }

	pub fn allocate_page(&mut self) -> Option<PhysicalPage> {
		if self.stats.allocated_pages == self.stats.total_pages - self.stats.reserved_pages {
			return None;
		}

		let mut bitmap = self.bitmap.0 as *mut u8;
		for byte in 0..(self.stats.total_pages >> 3) {
			unsafe {
				let byte = bitmap.add(byte as usize);
				if *byte != 0xff {
					for bit in 0..8 {
						if *byte & (1 << bit) == 0 {
							*byte |= 1 << bit;
							self.stats.allocated_pages += 1;
							let page = (byte as u64 - bitmap as u64) << 3 | bit;
							return Some(PhysicalPage(page << 12));
						}
					}
				}
			}
		}

		unsafe { unreachable_unchecked() }
	}

	pub unsafe fn deallocate_page(&mut self, page: PhysicalPage) {
		let page = page.0 >> 12;
		let byte = page >> 3;
		let bit = page & 7;
		let mut bitmap = self.bitmap.0 as *mut u8;
		unsafe {
			*bitmap.add(byte as usize) &= !(1 << bit);
		}
		self.stats.allocated_pages -= 1;
	}
}
