objs += $(APP)common/env.o


# Commands
objs += $(APP)common/cmd_bootm.o
objs += $(APP)common/cmd_fdt.o

# FIT image boot support
objs += $(APP)common/boot_image.o

# FDT Support
objs += $(APP)common/fdt_support.o
