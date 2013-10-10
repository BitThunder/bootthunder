MAKEFLAGS += --no-print-directory

APP_DIR="$(shell pwd)/"

all: .config
	@cp .config bitthunder/.config
	@$(MAKE) -C bitthunder APP_DIR=$(APP_DIR) APP_CONFIG=y

BOOT.BIN: bsp/bootthunder.elf
	@bash bootgen.sh

menuconfig:
	-@cp -f .config bitthunder/.config
	@$(MAKE) -C bitthunder APP_DIR=$(APP_DIR) APP_CONFIG=y menuconfig APP_BSP_CONFIG=y APP_BSP_DIR=../bsp
	@cp bitthunder/.config .config

.config:
	 @$(MAKE) menuconfig

clean:
	@$(MAKE) -C bitthunder APP_DIR=$(APP_DIR) APP_CONFIG=y clean
