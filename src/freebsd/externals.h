/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_FREEBSD_EXTERNALS_H
#define ICLOAK_FREEBSD_EXTERNALS_H 1

#include <sys/types.h>
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/module.h>
#include <sys/sysent.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/syscall.h>
#include <sys/sysproto.h>
#include <sys/malloc.h>
#include <sys/linker.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <vm/vm.h>
#include <vm/vm_page.h>
#include <vm/vm_map.h>

extern linker_file_list_t linker_files;

#if __FreeBSD__ > 6
// TIP(Rafael): In older versions it will not work because in older kernels we have 'struct mtx kld_mtx' instead.
extern struct sx kld_sx;

#define kld_lock sx_xlock(&kld_sx);

#define kld_unlock sx_xunlock(&kld_sx);

#else
extern struct mtx kld_mtx;

#define kld_lock mtx_lock(&kld_mtx);

#define kld_unlock mtx_unlock(&kld_mtx);

#endif

extern int next_file_id;

typedef TAILQ_HEAD(, module) modulelist_t;

extern modulelist_t modules;

extern int nextid;

struct module {
    TAILQ_ENTRY(module) link;
    TAILQ_ENTRY(module) flink;
    struct linker_file *file;
    int refs;
    int id;
    char *name;
    modeventhand_t handler;
    void *arg;
    modspecific_t data;
};

#endif
