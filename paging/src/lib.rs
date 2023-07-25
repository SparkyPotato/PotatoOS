#![no_std]

pub mod alloc;

#[derive(Copy, Clone, Debug, Eq, PartialEq)]
pub struct PhysicalPage(u64);
