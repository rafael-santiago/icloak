/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <linux/get_modules_list.h>
#include <linux/kallsyms.h>
#include <linux/version.h>

#define HAS_KALLSYMS_LOOKUP_NAME_EXPORT (LINUX_VERSION_CODE < KERNEL_VERSION(5,7,0))

#if !HAS_KALLSYMS_LOOKUP_NAME_EXPORT
# include <linux/kprobes.h>
static unsigned long long (*kallsyms_ln)(const char *) = NULL;
#endif

#define ICLOAK_LNX_MODULES_LIST_NAME "modules"

#if HAS_KALLSYMS_LOOKUP_NAME_EXPORT
void *get_modules_list(void) {
    return (void *)kallsyms_lookup_name(ICLOAK_LNX_MODULES_LIST_NAME);
}
#else
void *get_modules_list(void) {
    static struct kprobe kp_ctx = {
        .symbol_name = "kallsyms_lookup_name"
    };
    if (kallsyms_ln == NULL) {
        if (register_kprobe(&kp_ctx) < 0) {
            return NULL;
        }
        kallsyms_ln = (void *) kp_ctx.addr;
        unregister_kprobe(&kp_ctx);
    }
    return (kallsyms_ln != NULL) ? (void *) kallsyms_ln(ICLOAK_LNX_MODULES_LIST_NAME) : NULL;
}
#endif

#undef ICLOAK_LNX_MODULES_LIST_NAME
