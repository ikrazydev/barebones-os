#include "std.h"

#include <stdint.h>

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

void memmove(void* dest, const void* src, size_t size) {
    for (size_t i = 0; i < size; i++) {
        uint8_t *dest_ptr = (uint8_t*)dest + i, *src_ptr = (uint8_t*)src + i;
        *dest_ptr = *src_ptr;
    }
}
