/**
 *	FDT Modification Commands for BootThunder
 **/

#include <bootthunder.h>
#include <string.h>
#include <stdlib.h>
#include <libfdt.h>
#include <ctype.h>


static void do_usage() {

}

static void *g_fdt_addr = NULL;

static int fdt_parse_property(char **values, int count, char **data, int *len, int *bAllocated) {

	char *valp = values[0];
	int stridx = 0;
	*len = 0;
	*bAllocated = 0;

	if(values[0][0] == '<') {
		// Array of cells dec/hex
		// Assume data required == (count - 2) * 4 bytes + 8 for good measure.
		*data = BT_kMalloc((sizeof(BT_u32) * (count - 2)) + 8);
		*bAllocated = 1;

		char *dat = *data;

		valp++;
		while((stridx < count) && (*valp != '>')) {
			if(!*valp) {
				valp = values[++stridx];
				continue;
			}

			char *copy = valp;
			BT_u32 val = strtoul(valp, &valp, 0);
			*(BT_be32 *) dat = bt_cpu_to_be32(val);
			dat += 4;
			*len += 4;

			if((valp - copy) <= 0) {
				printf("Could not convert \"%s\"\n", copy);
				return -1;
			}
		}

		if(*valp != '>') {
			printf("Unexpected character %c\n", *valp);
			return -1;
		}
	} else if(values[0][0] == '[') {
		// Byte stream, (just hex)
		// Assume data required == (count - 2) * 1bytes, + 4 bytes for good measure
		*data = BT_kMalloc((sizeof(char) * (count - 2)) + 4);
		*bAllocated = 1;

		char *dat = *data;

		valp++;
		while((stridx < count) && (*valp != ']')) {
			if(!*valp) {
				valp = values[++stridx];
				continue;
			}

			if(!isxdigit((int)*valp)) {
				break;
			}

			int temp = strtoul(valp, &valp, 16);
			*dat++ = (char) (temp & 0xFF);
			*len    = *len + 1;
		}

		if(*valp != ']') {
			printf("Unexpected character '%c'\n", *valp);
			return -1;
		}

	} else {
		// Assume a string to be copied directly to data!
		*data = values[0];
		*bAllocated = 0;
		*len += strlen(values[0]) + 1;
	}

	return 0;
}

static int fdt_addr(int argc, char **argv) {
	if(argc != 1) {
		do_usage();
		return -1;
	}

	g_fdt_addr = (void *) strtoul(argv[0], NULL, 16);

	return 0;
}

static int fdt_set(int argc, char **argv) {

	int retval = 0;

	if(argc < 2) {
		do_usage();
		return -1;
	}

	char *path = argv[0];
	char *property = argv[1];
	char *data = NULL;

	int nodeoffset;
	int len = 0;

	nodeoffset = fdt_path_offset(g_fdt_addr, path);
	if(nodeoffset < 0) {
		printf("libfdt: fdt_path_offset() returned %s\n", fdt_strerror(nodeoffset));
		return -1;
	}

	if(argc == 3) {
		// Parse and allocate data as required.
	}

	int bAllocated = 0;
	retval = fdt_parse_property(argv+2, argc-2, &data, &len, &bAllocated);
	if(retval < 0) {
		bt_printf("Error parsing property value string!\n");
		goto err_out;
	}

	fdt_setprop(g_fdt_addr, nodeoffset, property, data, len);


err_out:
	if(bAllocated) {
		BT_kFree(data);
	}

	return retval;
}


typedef struct _DISPATCH_TABLE {
	const char *name;
	int (*cmd_fn) (int argc, char **argv);
} DISPATCH_TABLE;

static DISPATCH_TABLE g_Dispatch[] = {
	{"addr",	fdt_addr},
	{"set",		fdt_set},
};

/*
 *	Dispatch to the correct command handler.
 *
 */
static int cmd_fdt_dispatch(int argc, char **argv) {
	if(argc < 2) {
		do_usage();
		return -1;
	}


	DISPATCH_TABLE *pCommand = NULL;
	BT_u32 i;

	for(i = 0; i < sizeof(g_Dispatch)/sizeof(DISPATCH_TABLE); i++) {
		if(!strcmp(g_Dispatch[i].name, argv[1])) {
			pCommand = &g_Dispatch[i];
			break;
		}
	}

	if(pCommand) {
		return pCommand->cmd_fn(argc-2, argv+2);	// Pass arguments after the subcomand.
	}

	return -1;
}

BT_SHELL_COMMAND_DEF oCommand = {
	.szpName 	= "fdt",
	.eType 		= BT_SHELL_NORMAL_COMMAND,
	.pfnCommand = cmd_fdt_dispatch,
};
