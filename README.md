# BootThunder

A lightweight U-Boot replacement based on the BitThunder operating system.

BootThunder utilises the full power of the bitthunder operating system to create a simple
U-Boot like bootloader.

## Features

* Simple environment variables.
* Multi-core support, boot operating systems on any core.
* Device Tree - Supports modifications to an FDT blob at runtime.
* ATAGs - Support for legacy ARM tags.
* Fully supports booting of the latest linux Kernels.
* FPGA / Programmable logic support.
* FAT filesystem.
* SD Card.

## Supported Commands

* boot 		 - A simple application boot command, just jumps to start of an image.
* bootm		 - A fully featured image boot, with device tree and machine id passing.
* load		 - Loads any binary data to a specified address.
* load_fpga	 - Loads an fpga device with a bitstream.

## Planned Features

* Verified booting with CRC checking. (Like U-Boot's uImage).
* Netboot (convenient booting over TCP/IP with auto-discovery).
* Flash programming.
* Other filesystems (EXT2/4), JFFS etc.
