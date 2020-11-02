/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_FREEBSD_INCANTATIONS_H
#define ICLOAK_FREEBSD_INCANTATIONS_H 1

#include <sys/types.h>
#include <sys/sysproto.h>

int native_icloak_ko(const char *name);

int native_icloak_mk_ko_perm(const char *name, void **exit);

int native_icloak_mk_ko_nonperm(const char *name, void *exit);

int native_hide_file(const char *pattern);

int native_show_file(const char *pattern);

#endif
