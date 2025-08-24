#include "std.h"

#include <stdint.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }

    return len;
}

static inline void _memcpy_inline(void* dest, const void* src, size_t size) {
    size_t word_size = sizeof(size_t);

    uint8_t* dest_ptr = (uint8_t*)dest;
    const uint8_t* src_ptr = (const uint8_t*)src;

    while (size && ((uintptr_t)dest_ptr % word_size)) {
        *dest_ptr++ = *src_ptr++;
        size--;
    }

    size_t* wdest_ptr = (size_t*)dest_ptr;
    const size_t* wsrc_ptr = (const size_t*)src_ptr;

    while (size >= word_size) {
        *wdest_ptr++ = *wsrc_ptr++;
        size -= word_size;
    }

    dest_ptr = (uint8_t*)wdest_ptr;
    src_ptr = (const uint8_t*)wsrc_ptr;

    while (size--) {
        *dest_ptr++ = *src_ptr++;
    }
}

void memcpy(void* dest, const void* src, size_t size) {
    _memcpy_inline(dest, src, size);
}

static inline void _memmove_back(void* dest, const void* src, size_t size) {
    size_t word_size = sizeof(size_t);

    uint8_t* dest_ptr = (uint8_t*)dest + size;
    const uint8_t* src_ptr = (const uint8_t*)src + size;
    while (size && ((uintptr_t)dest_ptr % word_size)) {
        *--dest_ptr = *--src_ptr;
        --size;
    }

    size_t* wdest_ptr = (size_t*)dest_ptr;
    const size_t* wsrc_ptr = (const size_t*)src_ptr;
    while (size >= word_size) {
        *--wdest_ptr = *--wsrc_ptr;
        size -= word_size;
    }

    dest_ptr = (uint8_t*)wdest_ptr;
    src_ptr = (const uint8_t*)wsrc_ptr;
    while (size--) {
        *--dest_ptr = *--src_ptr;
    }
}

void memmove(void* dest, const void* src, size_t size) {
    if (dest == src || size == 0) return;

    if (dest < src) {
        _memcpy_inline(dest, src, size);
        return;
    }

    _memmove_back(dest, src, size);
}
