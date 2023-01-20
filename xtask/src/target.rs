use std::{path::PathBuf, process::Command};

use anyhow::{bail, Result};

use crate::opts::BuildOpt;

macro_rules! f {
    () => { impl FnOnce(PathBuf) -> Result<()> };
	($ty:ty) => { impl FnOnce(PathBuf, $ty) -> Result<()> };
}

pub enum Module {
	Bootloader,
	Kernel,
}

impl Module {
	pub fn name(&self) -> &'static str {
		match self {
			Module::Bootloader => "bootloader",
			Module::Kernel => "kernel",
		}
	}

	pub fn target(&self) -> &'static str {
		match self {
			Module::Bootloader => "x86_64-unknown-uefi",
			Module::Kernel => "x86_64-unknown-none",
		}
	}

	pub fn output(&self) -> &'static str {
		match self {
			Module::Bootloader => "bootloader.efi",
			Module::Kernel => "kernel",
		}
	}
}

pub struct CargoExecutor {
	pub cargo: String,
	pub target: String,
	pub opts: BuildOpt,
}

impl CargoExecutor {
	pub fn new(opts: BuildOpt) -> Self {
		Self {
			cargo: std::env::var("CARGO").unwrap(),
			target: std::env::var("CARGO_TARGET_DIR").unwrap_or_else(|_| "target".into()),
			opts,
		}
	}

	pub fn run(&self, module: Module, what: &str, with: f!()) -> Result<()> {
		let mut cmd = Command::new(&self.cargo);
		cmd.arg(what);
		if self.opts.release {
			cmd.arg("--release");
		}
		cmd.args(&["--package", module.name(), "--target", module.target()]);

		let status = cmd.status().unwrap();
		if !status.success() {
			bail!("build {} failed: {}", module.name(), status);
		}

		with(
			format!(
				"{}/{}/{}/{}",
				self.target,
				module.target(),
				self.opts.mode(),
				module.output()
			)
			.into(),
		)
	}

	pub fn pipeline<T>(&self, what: &str, ctx: &mut T, bootloader: f!(&mut T), kernel: f!(&mut T)) -> Result<()> {
		self.run(Module::Bootloader, what, |path| bootloader(path, ctx))?;
		self.run(Module::Kernel, what, |path| kernel(path, ctx))?;

		Ok(())
	}
}
