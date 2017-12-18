/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <linux/get_modules_list.h>
#include <linux/kallsyms.h>

#define ICLOAK_LNX_MODULES_LIST_NAME "modules"

void *get_modules_list(void) {
    return (void *)kallsyms_lookup_name(ICLOAK_LNX_MODULES_LIST_NAME);
}

#undef ICLOAK_LNX_MODULES_LIST_NAME
