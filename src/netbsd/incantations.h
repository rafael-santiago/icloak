/*
 *                                Copyright (C) 2018 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_NETBSD_INCANTATIONS_H
#define ICLOAK_NETBSD_INCANTATIONS_H 1

int native_icloak_ko(const char *name);

int native_hide_file(const char *pattern);

int native_show_file(const char *pattern);

#endif

