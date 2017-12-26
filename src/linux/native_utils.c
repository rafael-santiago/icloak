/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <linux/slab.h>

void icloak_native_free(void *ptr) {
    if (ptr != NULL) {
        kfree(ptr);
    }
}

void *icloak_native_alloc(size_t size) {
    return kmalloc(size, GFP_ATOMIC);
}
