/**
 *	Basic FDT support for BootThunder ...
 *	Wraps libfdt.
 **/

#include <bootthunder.h>
#include <libfdt.h>


/*
 *
 *
 */
int fdt_chosen(void *fdt, int force) {

	int error = 0;
	int nodeoffset;

	/*
	 *	Check fdt blob.
	 */
	error = fdt_check_header(fdt);
	if(error < 0) {
		printf("fdt_chosen: %s\n", fdt_strerror(error));
		return error;
	}

	/*
	 *
	 */
	nodeoffset = fdt_path_offset(fdt, "/chosen");
	if(nodeoffset < 0) {
		nodeoffset = fdt_add_subnode(fdt, 0, "chosen");	// Create a new node at root (0 offset is the root node).
		if(nodeoffset < 0) {
			printf("fdt_chosen: Could not create /chosen node: %s\n", fdt_strerror(nodeoffset));
			return nodeoffset;
		}
	}

	BT_ENV_VARIABLE *bootargs = BT_ShellGetEnv("bootargs");
	if(!bootargs) {
		return 0;
	}

	char *str = bootargs->o.string->s;
	const char *path = fdt_getprop(fdt, nodeoffset, "bootargs", NULL);
	if(!path || force) {
		error = fdt_setprop(fdt, nodeoffset, "bootargs", str, bootargs->o.string->length + 1);
		if(error < 0) {
			printf("fdt_chosen: Could not set bootargs: %s\n", fdt_strerror(error));
		}
	}

	return error;
}
