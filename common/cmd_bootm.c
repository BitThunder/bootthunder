/**
 *	A bootm command for BootThunder
 **/

#include <bootthunder.h>
#include <string.h>
#include <stdlib.h>

/*
 *	This command is designed for booting systems which use the Linux kernel
 *	boot-sequence.
 */
static int cmd_bootm(int argc, char **argv) {

	if(argc != 4 && argc != 6) {
		printf("Usage: %s {--core [coreID]} [start-addr] [initrd-addr | '-'] [fdt-addr | '-']\n", argv[0]);
		return -1;
	}

	BT_u32 	coreID 		= 0;
	BT_u32 	start_addr 	= 0;
	//BT_u32	initrd_addr = 0;
	BT_u32 	fdt_addr 	= 0;
	BT_u32	argoffset 	= 0;

	if(argc == 6) {
		argoffset = 2;
		if(strcmp(argv[1], "--core")) {
			printf("Invalid argument %s\n", argv[1]);
			return -1;
		}

		coreID = strtol(argv[2], NULL, 10);
	}

	start_addr 	= strtol(argv[argoffset+1], NULL, 16);
	//initrd_addr = strtol(argv[argoffset+2], NULL, 16);
	fdt_addr 	= strtol(argv[argoffset+3], NULL, 16);

	fdt_chosen((void *) fdt_addr, 1);	// Update the bootargs!

	// Here the initrd address should be placed into ATAGs or the FDT.

	return arch_bootm(coreID, (void *) start_addr, (void *) fdt_addr);
}

BT_SHELL_COMMAND_DEF oCommand = {
	.szpName 	= "bootm",
	.eType 		= BT_SHELL_NORMAL_COMMAND,
	.pfnCommand = cmd_bootm,
};
