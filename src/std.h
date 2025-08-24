#ifndef STD_H
#define STD_H

#include <stddef.h>

size_t strlen(const char* str);

void memcpy(void* dest, const void* src, size_t size);
void memmove(void* dest, const void* src, size_t size);

#endif
