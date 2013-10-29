/*
 *	Bootm for ARM.
 *
 *	This provides the platform specific implementation of the bootm command.
 */

#include <bootthunder.h>
#include <stdlib.h>

typedef void (*jump_regs)	(BT_u32 a, BT_u32 b, BT_u32 c, BT_u32 d);

int arch_bootm(BT_u32 coreID, void *start_addr, void *fdt_addr) {

	BT_u32 machine_id = 0;
	BT_ENV_VARIABLE *machid = BT_ShellGetEnv("machid");
	if(!machid) {
		printf("Warning : machine_id was not defined, set ${machid}:\n"
			   "e.g     : setenv machid 0xabad1dea\n");
	} else {
		machine_id = strtoul(machid->o.string->s, NULL, 16);
	}

	BT_DCacheFlush();
	BT_ICacheInvalidate();

	BT_DCacheDisable();

	if(coreID == BT_GetCoreID()) {
		BT_StopSystemTimer();
		BT_DisableInterrupts();

		jump_regs jumpr = (jump_regs) start_addr;
		jumpr(0, machine_id, (BT_u32) fdt_addr, 0);

		return -1;	// Is this such a good idea? Could be in an indefined state.

	} else {
		BT_BootCore(coreID, start_addr, 0, machine_id, (bt_register_t) fdt_addr, 0);		// Use the platform BootCore to run the boot_core function.
	}

	return 0;
}
