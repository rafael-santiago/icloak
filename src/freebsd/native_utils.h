/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_FREEBSD_NATIVE_UTILS_H
#define ICLOAK_FREEBSD_NATIVE_UTILS_H 1

#include <sys/types.h>

void *icloak_native_alloc(size_t size);

void icloak_native_free(void *ptr);

#endif
