#include <netbsd/native_utils.h>
#include <sys/malloc.h>

void *icloak_native_alloc(size_t size) {
	return malloc(size, M_TEMP, M_NOWAIT);
}

void icloak_native_free(void *ptr) {
	free(ptr, M_TEMP);
}


