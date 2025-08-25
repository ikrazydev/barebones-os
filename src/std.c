#include "std.h"

#include <stdint.h>

#define WORDSZ (sizeof(size_t))
#define LOBITS ((size_t)-1 / UINT8_MAX)
#define HIBITS (LOBITS * (UINT8_MAX/2+1))

size_t strlen(const char* str) {
    const char* start = str;
    while ((uintptr_t)str % WORDSZ) {
        if (!*str) {
            return str - start;
        }
        str++;
    }

    const size_t* wstr = (const void*)str;
    while (!((*wstr-LOBITS) & ~(*wstr) & HIBITS)) {
        wstr++;
    }

    str = (const void*)wstr;
    while (*str) {
        str++;
    }

    return str - start;
}

static inline void _memcpy_inline(void* dest, const void* src, size_t size) {
    uint8_t* dptr = (void*)dest;
    const uint8_t* sptr = (const void*)src;
    while (size && ((uintptr_t)dptr % WORDSZ)) {
        *dptr++ = *sptr++;
        size--;
    }

    size_t* wdptr = (void*)dptr;
    const size_t* wsptr = (const void*)sptr;
    while (size >= WORDSZ) {
        *wdptr++ = *wsptr++;
        size -= WORDSZ;
    }

    dptr = (void*)wdptr;
    sptr = (const void*)wsptr;
    while (size--) {
        *dptr++ = *sptr++;
    }
}

void memcpy(void* dest, const void* src, size_t size) {
    _memcpy_inline(dest, src, size);
}

static inline void _memmove_back(void* dest, const void* src, size_t size) {
    uint8_t* dptr = (uint8_t*)dest + size;
    const uint8_t* sptr = (const uint8_t*)src + size;
    while (size && ((uintptr_t)dptr % WORDSZ)) {
        *--dptr = *--sptr;
        --size;
    }

    size_t* wdptr = (size_t*)dptr;
    const size_t* wsptr = (const size_t*)sptr;
    while (size >= WORDSZ) {
        *--wdptr = *--wsptr;
        size -= WORDSZ;
    }

    dptr = (uint8_t*)wdptr;
    sptr = (const uint8_t*)wsptr;
    while (size--) {
        *--dptr = *--sptr;
    }
}

void memmove(void* dest, const void* src, size_t size) {
    if (dest < src) {
        _memcpy_inline(dest, src, size);
    } else {
        _memmove_back(dest, src, size);
    }
}
