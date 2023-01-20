use clap::{Parser, Subcommand};

#[derive(Parser)]
pub struct BuildOpt {
	#[clap(long)]
	pub release: bool,
}

impl BuildOpt {
	pub fn mode(&self) -> &'static str {
		if self.release {
			"release"
		} else {
			"debug"
		}
	}
}

#[derive(Subcommand)]
pub enum Action {
	Build(BuildOpt),
	Run(BuildOpt),
	Check,
}

#[derive(Parser)]
pub struct Opts {
	#[clap(subcommand)]
	pub action: Action,
}
