/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef ICLOAK_UTILS_ICLOAK_CTX_H
#define ICLOAK_UTILS_ICLOAK_CTX_H 1

#if defined(__linux__)
# include <linux/kernel.h>
#elif defined(__FreeBSD__)
# include <sys/types.h>
# include <sys/malloc.h>
# include <sys/systm.h>
#elif defined(__NetBSD__)
# include <sys/types.h>
# include <sys/malloc.h>
# include <sys/systm.h>
#endif

typedef struct icloak_filename_pattern {
    char *pattern;
    size_t pattern_size;
    struct icloak_filename_pattern *next;
}icloak_filename_pattern_ctx;

icloak_filename_pattern_ctx *icloak_add_filename_pattern(icloak_filename_pattern_ctx *head,
                                                         const char *pattern, size_t pattern_size,
                                                         icloak_filename_pattern_ctx **tail);

icloak_filename_pattern_ctx *icloak_del_filename_pattern(icloak_filename_pattern_ctx *head, const char *pattern);

void icloak_free_icloak_filename_pattern(icloak_filename_pattern_ctx *head);

int icloak_match_filename(const char *filename, const icloak_filename_pattern_ctx *head);

#endif

