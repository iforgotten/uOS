//
// Created by alvinli on 18-3-22.
//
#include "string.h"
#include "x86.h"

void*
memset(void *ptr, char c, size_t cnt)
{
#ifdef __HAVE_ARCH_MEMSET
    return __memset(ptr, c, cnt);
#else
    char* dst = (char*)ptr;
    int i;
    while(++i <= cnt) {
        *dst = c;
        ++dst;
    }
    return dst;
#endif
}

void*
memmove(void* dst, void* src, size_t cnt) {
#ifdef __HAVE_ARCH_MEMMOVE
	return __memmove(dst, src, cnt);
#else
	void* ret = dst;
	int i = 0;
	while(i < cnt) {
		dst[i] = src[i];
		++i;
	}
	return ret;
#endif
}
