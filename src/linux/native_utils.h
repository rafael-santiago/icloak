/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_LINUX_NATIVE_UTILS_H
#define ICLOAK_LINUX_NATIVE_UTILS_H 1

#include <linux/kernel.h>

void icloak_native_free(void *ptr);

void *icloak_native_alloc(size_t size);

#endif
