/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_H
#define ICLOAK_H 1

#if defined(__linux__)
# include <linux/incantations.h>
# elif defined(__FreeBSD__)
// TODO(Rafael): The FreeBSD incantations.
# else
# error "Not supported platform."
# endif

#define icloak_ko(name) native_icloak_ko(name)

#define icloak_mk_ko_perm(name, exit) native_icloak_mk_ko_perm(name, exit)

#define icloak_mk_ko_nonperm(name, exit) native_icloak_mk_ko_nonperm(name, exit)

#endif
