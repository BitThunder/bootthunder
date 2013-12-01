MAKEFLAGS += --no-print-directory

APP_DIR="$(shell pwd)/"

all: .config
	@cp .config bitthunder/.config
	@$(MAKE) -C bitthunder APP_DIR=$(APP_DIR) APP_CONFIG=y

BOOT.BIN: all
	@cp $(APP_DIR)bsp/bootthunder.elf bitthunder/arch/arm/mach/zynq/tools/
	@cd bitthunder/arch/arm/mach/zynq/tools/ && python ./bootgen.py temp.bif
	@cp bitthunder/arch/arm/mach/zynq/tools/BOOT.BIN $(APP_DIR)

menuconfig:
	-@cp -f .config bitthunder/.config
	@$(MAKE) -C bitthunder APP_DIR=$(APP_DIR) APP_CONFIG=y menuconfig APP_BSP_CONFIG=y APP_BSP_DIR=../bsp
	@cp bitthunder/.config .config

.config:
	 @$(MAKE) menuconfig

clean:
	@$(MAKE) -C bitthunder APP_DIR=$(APP_DIR) APP_CONFIG=y clean

.PHONY: BOOT.BIN

