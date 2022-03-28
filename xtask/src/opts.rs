use clap::{Parser, Subcommand};

#[derive(Parser)]
pub struct BuildOpt {
	#[clap(long)]
	pub release: bool,
}

#[derive(Subcommand)]
pub enum Action {
	Build(BuildOpt),
}

#[derive(Parser)]
pub struct Opts {
	#[clap(subcommand)]
	pub action: Action,
}
