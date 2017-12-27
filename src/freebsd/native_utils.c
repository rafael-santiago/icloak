/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <freebsd/native_utils.h>
#include <sys/malloc.h>

void *icloak_native_alloc(size_t size) {
    return malloc(size, M_TEMP, M_NOWAIT);
}

void icloak_native_free(void *ptr) {
    free(ptr, M_TEMP);
}
