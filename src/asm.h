#ifndef ASM_H
#define ASM_H

#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
#ifdef __GNUC__ // Different syntax for other compilers
    uint8_t ret;
    __asm__ volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
#endif // __GNUC__
}

static inline void outb(uint16_t port, uint8_t value) {
#ifdef __GNUC__
    __asm__ volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
#endif // __GNUC__
}

#endif
