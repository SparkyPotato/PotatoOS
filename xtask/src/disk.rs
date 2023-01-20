use std::{
	collections::HashMap,
	fs::OpenOptions,
	io::{Cursor, Seek, SeekFrom, Write},
	path::Path,
};

use anyhow::{bail, Result};
use common::POTATO_OS_PARTITION_TYPE_UUID_STR;
use fatfs::{Dir, FileSystem, FormatVolumeOptions, FsOptions, ReadWriteSeek};
use gpt::{
	mbr::ProtectiveMBR,
	partition_types::{OperatingSystem, Type},
};

use crate::opts::BuildOpt;

pub enum EFIEntry {
	File(Vec<u8>),
	Dir(HashMap<String, EFIEntry>),
}

pub struct Disk {
	disk: String,
	efi_root: EFIEntry,
	efi_size: u64,
	kernel: Vec<u8>,
}

impl Disk {
	pub fn new(target: &str, opts: &BuildOpt) -> Result<Self> {
		Ok(Self {
			disk: format!("{}/{}/Disk.img", target, opts.mode()),
			efi_root: EFIEntry::Dir(HashMap::new()),
			efi_size: 0,
			kernel: Vec::new(),
		})
	}

	pub fn copy_efi(&mut self, from: impl AsRef<Path>, to: impl AsRef<str>) -> Result<()> {
		let mut entry = &mut self.efi_root;
		for elem in to.as_ref().split('/') {
			match entry {
				EFIEntry::File(_) => bail!("{} is a file", to.as_ref()),
				EFIEntry::Dir(dir) => {
					entry = dir.entry(elem.into()).or_insert(EFIEntry::Dir(HashMap::new()));
				},
			}
		}
		let data = std::fs::read(from)?;
		self.efi_size += data.len() as u64;
		*entry = EFIEntry::File(data);

		Ok(())
	}

	pub fn write_kernel(&mut self, data: Vec<u8>) { self.kernel = data; }

	pub fn write(&mut self) -> Result<String> {
		const MB: u64 = 1024 * 1024;
		const KB: u64 = 1024;

		// Generate EFI FAT32 image.
		let efi_size = ((self.efi_size + 64 * KB - 1) / MB + 1) * MB;
		let mut efi_data = vec![0; efi_size as usize];
		{
			let fmt_options = FormatVolumeOptions::new().volume_label(*b"EFI\0\0\0\0\0\0\0\0");
			fatfs::format_volume(Cursor::new(&mut efi_data), fmt_options).unwrap();
			let fs = FileSystem::new(Cursor::new(&mut efi_data), FsOptions::new())?;

			let out_dir = fs.root_dir();
			let dir = match self.efi_root {
				EFIEntry::Dir(ref mut x) => x,
				EFIEntry::File(_) => unreachable!(),
			};
			Self::recursive_fat_write(out_dir, dir)?;
		}

		// Generate GPT disk.
		{
			let kernel_size = self.kernel.len() as u64;
			let size = 64 * KB + efi_size + kernel_size;

			let mut disk = OpenOptions::new()
				.create(true)
				.truncate(true)
				.read(true)
				.write(true)
				.open(&self.disk)?;
			disk.set_len(size)?;

			ProtectiveMBR::with_lb_size(u32::try_from(size / 512 - 1).unwrap_or(0xFF_FF_FF_FF))
				.overwrite_lba0(&mut disk)?;

			let block_size = gpt::disk::LogicalBlockSize::Lb512;
			let mut gpt = gpt::GptConfig::new()
				.writable(true)
				.initialized(false)
				.logical_block_size(block_size)
				.create_from_device(Box::new(&mut disk), None)?;
			gpt.update_partitions(Default::default())?;

			let partition_id = gpt.add_partition("EFI", efi_size, gpt::partition_types::EFI, 0, None)?;
			let partition = gpt.partitions().get(&partition_id).unwrap();
			let efi_start_offset = partition.bytes_start(block_size)?;

			let partition_id = gpt.add_partition(
				"Boot",
				kernel_size,
				Type {
					guid: POTATO_OS_PARTITION_TYPE_UUID_STR,
					os: OperatingSystem::Custom("PotatoOS".to_string()),
				},
				0,
				None,
			)?;
			let partition = gpt.partitions().get(&partition_id).unwrap();
			let boot_start_offset = partition.bytes_start(block_size)?;

			gpt.write()?;

			// Write partitions.
			disk.seek(SeekFrom::Start(efi_start_offset))?;
			std::io::copy(&mut efi_data.as_slice(), &mut disk)?;

			disk.seek(SeekFrom::Start(boot_start_offset))?;
			disk.write_all(&self.kernel)?;
		}

		Ok(self.disk.clone())
	}

	fn recursive_fat_write<T>(odir: Dir<T>, dir: &mut HashMap<String, EFIEntry>) -> Result<()>
	where
		T: ReadWriteSeek,
	{
		for (name, entry) in dir {
			match entry {
				EFIEntry::File(data) => {
					let mut file = odir.create_file(name)?;
					file.truncate()?;
					file.write_all(&data)?;
				},
				EFIEntry::Dir(ref mut dir) => {
					let odir = odir.create_dir(name)?;
					Self::recursive_fat_write(odir, dir)?;
				},
			}
		}

		Ok(())
	}
}
