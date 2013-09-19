#ifndef _FDT_SUPPORT_H_
#define _FDT_SUPPORT_H_

int fdt_find_and_set(void *fdt, const char *node, const char *prop, const void *val, int len, int create);
int fdt_chosen(void *fdt, int force);

#endif
