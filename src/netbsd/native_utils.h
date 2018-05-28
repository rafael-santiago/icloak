#ifndef ICLOAK_NETBSD_NATIVE_UTILS_H
#define ICLOAK_NETBSD_NATIVE_UTILS_H 1

#include <sys/types.h>

void *icloak_native_alloc(size_t size);

void icloak_native_free(void *ptr);

#endif

