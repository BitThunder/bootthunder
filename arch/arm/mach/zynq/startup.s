.extern _bt_start
.extern ps7_init
.globl _bt_startup_boot
_bt_startup_boot:
	ldr	r0,=1
	ldr	r1,=0xf8f00208		@ Start the global timer.
	str	r0,[r1]
	ldr	r0,=0xDEADBEEF
	@bl	ps7_init			@ Run the clock configurator, as exported from Xilinx HW generator.
	@bl ps7_post_config
	bx	lr
	mov	r0, r0
.globl _sbrk
_sbrk:
	mov	pc, lr
/*
.globl ps7_post_config
.weak ps7_post_config
ps7_post_config:
	mov	pc, lr*/
