objs += $(APP)bootthunder.o


include $(APP_DIR)common/objects.mk

$(objs): MODULE_NAME="APP|test"
