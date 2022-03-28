use std::process::Command;

use anyhow::{bail, Result};
use clap::Parser;

use crate::opts::{Action, BuildOpt, Opts};

mod opts;

fn build(opts: BuildOpt) -> Result<()> {
	let mut cmd = Command::new(std::env::var("CARGO").unwrap());
	cmd.arg("build");

	if opts.release {
		cmd.arg("--release");
	}

	cmd.args(&[
		"--target",
		"x86_64-unknown-uefi",
		"-Zbuild-std=core,compiler_builtins,alloc",
		"-Zbuild-std-features=compiler-builtins-mem",
	]);

	cmd.args(&["--package", "bootloader"]);

	let status = cmd.status().unwrap();
	if !status.success() {
		bail!("build bootloader failed: {}", status);
	}

	let target = std::env::var("CARGO_TARGET_DIR").unwrap_or_else(|_| "target".into());

	std::fs::create_dir_all(format!(
		"{}/{}/boot/EFI/Boot",
		target,
		if opts.release { "release" } else { "debug" }
	))?;

	std::fs::copy(
		format!(
			"{}/x86_64-unknown-uefi/{}/bootloader.efi",
			target,
			if opts.release { "release" } else { "debug" }
		),
		format!(
			"{}/{}/boot/EFI/Boot/Bootx64.efi",
			target,
			if opts.release { "release" } else { "debug" }
		),
	)?;

	Ok(())
}

fn main() -> Result<()> {
	let opts = Opts::parse();
	match opts.action {
		Action::Build(opts) => build(opts),
	}
}
