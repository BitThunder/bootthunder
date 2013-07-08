
# libfdt
objs_fdt += $(APP)lib/dtc/libfdt/fdt.o
objs_fdt += $(APP)lib/dtc/libfdt/fdt_ro.o
objs_fdt += $(APP)lib/dtc/libfdt/fdt_wip.o
objs_fdt += $(APP)lib/dtc/libfdt/fdt_sw.o
objs_fdt += $(APP)lib/dtc/libfdt/fdt_rw.o
objs_fdt += $(APP)lib/dtc/libfdt/fdt_strerror.o
objs_fdt += $(APP)lib/dtc/libfdt/fdt_empty_tree.o

CFLAGS += -I $(APP_DIR)lib/dtc/libfdt/



$(objs_fdt): CFLAGS += -I $(APP_DIR)lib/dtc/libfdt/
$(objs_fdt): MODULE_NAME="libfdt"

objs += $(objs_fdt)
