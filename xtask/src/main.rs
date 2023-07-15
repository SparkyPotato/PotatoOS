use std::process::Command;

use anyhow::{anyhow, bail, Context, Result};
use clap::Parser;

use crate::{
	disk::Disk,
	opts::{Action, BuildOpt, Opts},
	target::CargoExecutor,
};

mod disk;
mod opts;
mod target;

pub struct BuildContext {
	disk: Disk,
	executor: CargoExecutor,
}

impl BuildContext {
	fn new(opts: BuildOpt) -> Self {
		let executor = CargoExecutor::new(opts);
		let disk = Disk::new(&executor.target, &executor.opts).unwrap();
		Self { disk, executor }
	}

	fn build(&mut self) -> Result<String> {
		// Build and copy bootloader.
		self.executor.pipeline(
			"build",
			&mut self.disk,
			|path, disk| disk.copy_efi(path, "EFI/Boot/Bootx64.efi"),
			|path, disk| Ok(disk.write_kernel(std::fs::read(path)?)),
		)?;

		self.disk.write().with_context(|| "failed to write disk image")
	}

	fn run(&mut self) -> Result<()> {
		let disk = self.build()?;

		let mut cmd = Command::new("qemu-system-x86_64");
		if cfg!(target_os = "linux") {
			cmd.arg("-enable-kvm");
		}

		// OVMF
		std::env::var("OVMF")
			.map(|ovmf| {
				cmd.args(&[
					"-drive",
					&format!("if=pflash,format=raw,readonly=on,file={}/OVMF_CODE.fd", ovmf),
				]);
				cmd.args(&["-drive", &format!("if=pflash,format=raw,file={}/OVMF_VARS.fd", ovmf)]);
			})
			.map_err(|_| {
				anyhow!(
					"`OVMF` environment variable not set, set to a directory containing `OVMF_CODE.fd` and \
					 `OVMF_VARS.fd`"
				)
			})?;

		// Us
		cmd.args(["-drive", format!("format=raw,file={}", disk).as_str()]);

		match cmd.spawn() {
			Ok(mut c) => {
				let _ = c.wait();
			},
			Err(e) => {
				if matches!(e.kind(), std::io::ErrorKind::NotFound) {
					bail!("qemu-system-x86_64 not found in PATH, please install qemu");
				} else {
					bail!("failed to spawn qemu: {}", e);
				}
			},
		}

		Ok(())
	}
}

fn main() -> Result<()> {
	let opts = Opts::parse();
	match opts.action {
		Action::Build(opts) => {
			let _ = BuildContext::new(opts).build()?;
			Ok(())
		},
		Action::Run(opts) => BuildContext::new(opts).run(),
		Action::Check => {
			let c = |_, _: &mut ()| Ok(());
			CargoExecutor::new(BuildOpt { release: false }).pipeline("check", &mut (), c, c)
		},
	}
}
