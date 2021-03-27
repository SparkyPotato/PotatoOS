#![allow(non_snake_case)]
#![no_std]
#![no_main]
#![feature(abi_efiapi)]

use uefi::prelude::*;

#[entry]
fn efi_main(_handle: Handle, systemTable: SystemTable<Boot>) -> Status
{
    uefi_services::init(&systemTable).expect_success("Failed to initialize utilities");
    let stdout = systemTable.stdout();
    stdout.reset(false).expect("Failed to reset stdout").expect("Failed to reset stdout");

    let runtime = systemTable.runtime_services();
    runtime.reset(uefi::table::runtime::ResetType::Shutdown, Status::SUCCESS, None);
}
