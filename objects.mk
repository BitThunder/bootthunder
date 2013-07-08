objs += $(APP)bootthunder.o


include $(APP_DIR)common/objects.mk


-include $(APP_DIR)arch/$(ARCH)/objects.mk
-include $(APP_DIR)arch/$(ARCH)/mach/$(SUBARCH)/objects.mk


$(objs): MODULE_NAME="BootThunder"
$(objs): CFLAGS += -I $(APP_DIR)include


#
#	Libraries
#
include $(APP_DIR)lib/objects.mk
