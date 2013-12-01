#
#	BSP Makefile for BootThunder project.
#

BASE=$(shell pwd)/../bitthunder/
BUILD_BASE=$(BASE)
MODULE_NAME="bootthunder"

TARGETS=boothunder.img
TARGET_DEPS=bootthunder.elf


BUILD_DIR=$(shell pwd)/build/

TOOLCHAIN=/opt/codesourcery/bin/arm-none-eabi-

include $(BASE).dbuild/dbuild.mk

tool:
	@echo $(BASE)
	@echo $(BT_CONFIG_SUBARCH)
	@echo $(BT_CONFIG_TOOLCHAIN)
	@echo $(TOOLCHAIN)

all: bootthunder.elf bootthunder.list bootthunder.img bootthunder.syms
	$(Q)$(SIZE) bootthunder.elf

bootthunder.img: bootthunder.elf
	$(Q)$(PRETTY) IMAGE $(MODULE_NAME) $@
	$(Q)$(OBJCOPY) bootthunder.elf -O binary $@

bootthunder.list: bootthunder.elf
	$(Q)$(PRETTY) LIST $(MODULE_NAME) $@
	$(Q)$(OBJDUMP) -D bootthunder.elf > $@

bootthunder.syms: bootthunder.elf
	$(Q)$(PRETTY) SYMS $(MODULE_NAME) $@
	$(Q)$(OBJDUMP) -t bootthunder.elf > $@

bootthunder.elf: $(OBJECTS)
	$(Q)$(PRETTY) --dbuild "LD" $(MODULE_NAME) $@
	$(Q)$(CC) -o $@ -T $(LINKER_SCRIPT) -Wl,-Map=bootthunder.map -Wl,--gc-sections $(OBJECTS) -nostdlib -lc -lgcc 
