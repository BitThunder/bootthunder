/**
 *	Image booting for BootThunder.
 *
 *	BootThunder support Flattened Image Tree images for the maximum flexibility possible.
 *
 **/
#include <bootthunder.h>
#include <libfdt.h>
#include <hash/bt_crc.h>
#include <hash/bt_md5.h>


static const char *fit_get_description(const void *fit, int node) {
	return fdt_getprop(fit, node, "description", NULL);
}

static int verify_checksum(const void *data, BT_u32 len, const char *algo, const void *digest, int dig_len) {
#ifdef BT_CONFIG_BOOTTHUNDER_HASH_CRC32
	if(!strcmp(algo, "crc32")) {
		BT_u8 vdigest[4];
		BT_u32 crc32;
		memcpy(&crc32, digest, 4);
		crc32 = bt_be32_to_cpu(crc32);

		bt_crc32(data, len, vdigest);
		if(memcmp(&crc32, vdigest, 4)) {
			return -1;
		}

		return 0;

	}
#endif
	if(!strcmp(algo, "md5")) {
		BT_u8 vdigest[16];
		bt_md5(data, len, vdigest);
		if(memcmp(digest, vdigest, 16)) {
			return -1;
		}

		return 0;
	} 

	if(!strcmp(algo, "sha1")) {
		
	}

	return 1;	// unsupported algorithm
}

static int get_image_node(void *fit, int imagesnode, const char *image_name) {
	int inode;
	for(inode = fdt_first_subnode(fit, imagesnode); inode >= 0; inode = fdt_next_subnode(fit, inode)) {
		if(!strcmp(image_name, fdt_get_name(fit, inode, NULL))) {
			break;
		}
	}

	return inode;
}

static int verify_image_node(BT_HANDLE hStdout, void *fit, int image_node) {
	int len;
	const void *data = fdt_getprop(fit, image_node, "data", &len);

	bt_fprintf(hStdout, "   Verifying Hash Integrity : ");
	
	int hnode;
	int fail = 0;
	int verified = 0;
	int count;
	for(count = 0, hnode = fdt_first_subnode(fit, image_node); hnode >= 0; hnode = fdt_next_subnode(fit, hnode), count++) {
		const char *algo = fdt_getprop(fit, hnode, "algo", NULL);
		if(!algo) {
			continue;
		}

		bt_fprintf(hStdout, "%s", algo);

		int dig_len;
		const void *digest = fdt_getprop(fit, hnode, "value", &dig_len);
		if(!digest) {
			bt_fprintf(hStdout, "! ");	// No digest available!
			continue;
		}

		int ret;
		if((ret = verify_checksum(data, len, algo, digest, dig_len)) < 0) {
			bt_fprintf(hStdout, "- ");
			fail = 1;
		} else if (ret) {
			bt_fprintf(hStdout, "? ");
		} else {
			bt_fprintf(hStdout, "+ ");
			verified = 1;
		}		
	}

	if(fail) {
		bt_fprintf(hStdout, "FAIL");		
	} else if(verified) {
		bt_fprintf(hStdout, "OK");
	} else {
		bt_fprintf(hStdout, "UNAVAILABLE");
	}

	bt_fprintf(hStdout, "\n");

	if(!count) {
		bt_fprintf(hStdout, "   WARNING: Image contains no checksums, cannot verify image integrity\n");
	}

	if(!verified && !fail) {
		bt_fprintf(hStdout, "   WARNING: Image contains no supported checksums, cannot verify integrity\n");
	}

	if(fail) {
		bt_fprintf(hStdout, "   ERROR: Image checksum failed for %s (%s)\n", fdt_get_name(fit, image_node, NULL), fdt_getprop(fit, image_node, "description", NULL));
		return -1;
	}

	return 0;
}

static int load_image_node(BT_HANDLE hStdout, void *fit, int image_node, int config_node) {
	int len;
	const void *data = fdt_getprop(fit, image_node, "data", &len);
	const char *type = fdt_getprop(fit, image_node, "type", NULL);
	if(!type) {
		return -1;
	}
	
	if(!strcmp(type, "bitstream")) {
		const char *fpga_dev = fdt_getprop(fit, config_node, "fpga_device", NULL);
		if(!fpga_dev) {
			bt_fprintf(hStdout, "ERROR: fpga device not specified for fpga image in configuration %s (%s)\n", fdt_get_name(fit, config_node, NULL), fdt_getprop(fit, config_node, "description", NULL));
			return -1;
		}

		BT_HANDLE hDevice = BT_Open(fpga_dev, BT_GetModeFlags("wb+"), NULL);
		if(!hDevice) {
			bt_fprintf(hStdout, "ERROR: Could not open fpga device (%s)\n", fpga_dev);
			return -1;
		}


		#define FPGA_BUF	1024*512
		const BT_u8 *pdata = data;

		BT_u32 written = 0;
		while(written < len) {
			BT_u32 wrote = BT_Write(hDevice, 0, FPGA_BUF, pdata, NULL);
			pdata += wrote;
			written += wrote;
		}

		BT_CloseHandle(hDevice);
		
		return 0;
	}

	if(!strcmp(type, "kernel")) {
		const void *load_addr = fdt_getprop(fit, image_node, "load", NULL);
		if(!load_addr) {
			return -1;
		}

		void *load = (void *) bt_be32_to_cpu(*((BT_u32 *) load_addr));
		memcpy(load, data, len);
	}
	
	return 0;
}

int boot_image(BT_HANDLE hStdout, void *image, const char *config) {

	if(fdt_check_header(image)) {
		bt_fprintf(hStdout, "Invalid image header, no FIT image detected at 0x%08x\n", image);
		return -1;
	}

	int image_node = fdt_path_offset(image, "/images");
	if(image_node < 0) {
		bt_fprintf(hStdout, "Image is not a valid FIT, it contains no /images node\n");
		return -1;
	}

	int cnode = fdt_path_offset(image, "/configurations");
	if(cnode < 0) {
		bt_fprintf(hStdout, "Image is not a valid FIT, it contains no /configurations node\n");
		return -1;
	}

	const char *defconfig = fdt_getprop(image, cnode, "default", NULL);
	if(!defconfig && !config) {
		bt_fprintf(hStdout, "Warning: FIT image has no default configuration, choosing first configuration in tree.\n");
	}

	if(!config) {
		config = defconfig;
	}

	int node;
	const char *name = NULL;
	for(node = fdt_first_subnode(image, cnode); node >= 0; node = fdt_next_subnode(image, node)) {
		name = fdt_get_name(image, node, NULL);

		if(!config) {
			break;		// Break immediately, and selected node is the loadable configuration.
		}
		
		if(!strcmp(name, config)) {
			break;	   	// Matching configuration node found.
		}
	}

	if(node < 0) {
		bt_fprintf(hStdout, "No valid configuration was found in FIT image\n");
		return -1;
	}

	bt_fprintf(hStdout, "## Booting with FIT image using %s configuration\n", name);	

	// Handle fpga image.
	const char *fpga = fdt_getprop(image, node, "fpga", NULL);
	if(fpga) {
		bt_fprintf(hStdout, "## FPGA from FIT image\n");
		int fpga_node = get_image_node(image, image_node, fpga);
		if(fpga_node >= 0) {
			const char *description = fit_get_description(image, fpga_node);
			bt_fprintf(hStdout, "   Description : %s\n", description);			
			if(verify_image_node(hStdout, image, fpga_node)) {
				return -1;
			}
			load_image_node(hStdout, image, fpga_node, node);
		} else {
			bt_fprintf(hStdout, "   ERROR: no matching image (%s) for fpga in config %s\n", fpga, name);
			return -1;
		}
	}

	// Handle kernel images.
	const char *kernel = fdt_getprop(image, node, "kernel", NULL);
	void *start_addr = NULL;
	if(kernel) {
		int kernel_node = get_image_node(image, image_node, kernel);
		if(kernel_node >= 0) {
			if(verify_image_node(hStdout, image, kernel_node)) {
				return -1;
			}
			load_image_node(hStdout, image, kernel_node, node);
			
			start_addr = (void *) fdt_getprop(image, kernel_node, "entry", NULL);
			if(!start_addr) {
				return -1;
			}

			start_addr = (void *) bt_be32_to_cpu(*((BT_u32 *) start_addr));

		} else {
			bt_fprintf(hStdout, "ERROR: no matching image (%s) for kernel in config %s\n", kernel, name);
			return -1;
		}
	}

	// Handle fdt image. (Second incase fdt is embedded within kernel image i.e. whitespace at start of image).
	const char *fdt = fdt_getprop(image, node, "fdt", NULL);
	void *fdt_addr = NULL;
	if(fdt) {
		int fdt_node = get_image_node(image, image_node, fdt);
		if(fdt_node >= 0) {
			if(verify_image_node(hStdout, image, fdt_node)) {
				return -1;
			}
			load_image_node(hStdout, image, fdt_node, node);

			fdt_addr = (void *) fdt_getprop(image, fdt_node, "load", NULL);
			if(!fdt_addr) {
				return -1;
			}

			fdt_addr = (void *) bt_be32_to_cpu(*((BT_u32 *) fdt_addr));

		} else  {
			bt_fprintf(hStdout, "ERROR: no matching image (%s) for kernel in config %s\n", kernel, name);
			return -1;
		}
	}

	// Other processing options available later, e.g. like file copy etc.

	// Bootm with the full parameters.
	if(kernel) {
		return arch_bootm(0, start_addr, fdt_addr);
	}

	return 0;
}
