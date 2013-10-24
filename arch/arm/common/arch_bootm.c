/*
 *	Bootm for ARM.
 *
 *	This provides the platform specific implementation of the bootm command.
 */

#include <bootthunder.h>
#include <stdlib.h>

typedef void (*jump) 		(void);
typedef void (*jump_regs)	(BT_u32 a, BT_u32 b, BT_u32 c, BT_u32 d);

typedef struct _BOOT_PARAMS {
	jump jmp;
	void *start_addr;
	void *initrd_addr;
	void *fdt_addr;
	BT_u32	machine_id;
} BOOT_PARAMS;

static BOOT_PARAMS oBootParams[BT_CONFIG_CPU_CORES];

static void boot_core(void) {

	__asm volatile("ldr sp,=0x30000");				// There's probably something more sensible we could do!

	register BT_u32 a,b,c,d;
	BT_u32 coreID = BT_GetCoreID();

	a = 0;
	b = oBootParams[coreID].machine_id;
	c = (BT_u32) oBootParams[coreID].fdt_addr;
	d = 0;

	jump_regs jmp = (jump_regs) oBootParams[coreID].jmp;
	jmp(a, b, c, d);
}

int arch_bootm(BT_u32 coreID, void *start_addr, void *initrd_addr, void *fdt_addr) {

	char *s = NULL;
	BT_ENV_VARIABLE *machid = BT_ShellGetEnv("machid");
	if(!machid) {
		oBootParams[coreID].machine_id = 0;
		printf("Warning : machine_id was not defined, set ${machid}:\n"
			   "e.g     : setenv machid 0xCAFED00D\n");
	} else {
		s = machid->o.string->s;
		//printf("Using machine I 0x%08X from ${machid}\n");
	}

	if(s) {
		oBootParams[coreID].machine_id = strtoul(s, NULL, 16);
	}

	BT_DCacheFlush();
	BT_ICacheInvalidate();

	BT_DCacheDisable();

	oBootParams[coreID].jmp = start_addr;
	oBootParams[coreID].initrd_addr = initrd_addr;
	oBootParams[coreID].fdt_addr = fdt_addr;

	if(coreID == BT_GetCoreID()) {
		BT_StopSystemTimer();
		BT_DisableInterrupts();
		boot_core();

		return -1;	// Is this such a good idea? Could be in an indefined state.

	} else {
		BT_BootCore(coreID, start_addr, 0, machine_id, (bt_register_t) fdt_addr, 0);		// Use the platform BootCore to run the boot_core function.
	}

	return 0;
}
