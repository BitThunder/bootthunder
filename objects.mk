objs += $(APP)bootthunder.o


include $(APP_DIR)common/objects.mk



-include $(APP_DIR)arch/$(ARCH)/mach/$(SUBARCH)/objects.mk


$(objs): MODULE_NAME="APP|test"
