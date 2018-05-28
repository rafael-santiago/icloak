/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_UTILS_MEMORY_H
#define ICLOAK_UTILS_MEMORY_H 1

#if defined(__linux__)
# include <linux/native_utils.h>
#elif defined(__FreeBSD__)
# include <freebsd/native_utils.h>
#elif defined(__NetBSD__)
# include <netbsd/native_utils.h>
#endif

#define icloak_free(p) icloak_native_free(p)

#define icloak_alloc(s) icloak_native_alloc(s)

#endif
