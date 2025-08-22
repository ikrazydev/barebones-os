#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "asm.h"
#include "std.h"
#include "vga.h"

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_cursormove(size_t x, size_t y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0x00FF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos & 0xFF00) >> 8));
}

void terminal_updatecursor(void) {
    terminal_cursormove(terminal_column, terminal_row);
}

void terminal_shiftdown(void) {
    terminal_row--;
    
    size_t last_row = VGA_WIDTH * (VGA_HEIGHT - 1);
    memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, last_row * sizeof(uint16_t));

    for (size_t i = last_row; i < last_row + VGA_WIDTH; i++) {
        terminal_buffer[i] = vga_entry(' ', terminal_color);
    }
}

void terminal_newline(void) {
    terminal_column = 0;

    if (++terminal_row == VGA_HEIGHT) {
        terminal_shiftdown();
    }
}

void terminal_erase(void) {
    if (terminal_column == 0) {
        if (terminal_row != 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
    } else {
        terminal_column--;
    }

    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
    terminal_updatecursor();
}

void terminal_putchar(char c) {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_newline();
    }

    terminal_updatecursor();
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        const char ch = data[i];

        if (ch == '\r') continue;
        else if (ch == '\n') {
            terminal_newline();
        }
        else if (ch == '\b') {
            terminal_erase();
        }
        else {
            terminal_putchar(ch);
        }
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void terminal_writenumber(int num) {
    char temp[16];
    char* tp = temp;

    while (num > 0) {
        int digit = num % 10;
        *tp++ = digit;

        num /= 10;
    }

    while (tp > temp) {
        char ch = *--tp + '0';
        terminal_write(&ch, 1);
    }
}
