#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "asm.h"
#include "std.h"
#include "vga.h"

static uint16_t* vga_buffer = (uint16_t*)VGA_MEMORY;

#define MAX_LINES 100

static uint16_t lines[MAX_LINES][VGA_WIDTH];
static size_t line_lengths[MAX_LINES];

static size_t top_line;
static size_t row;
static size_t max_row;
static size_t column;
static size_t saved_column;
static uint8_t color;

void terminal_initialize(void) {
    top_line = 0;
    row = 0; max_row = 0;
    column = 0; saved_column = 0;
    color = vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);

    for (size_t y = 0; y < MAX_LINES; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', color);

            lines[y][x] = vga_buffer[index];
        }

        line_lengths[y] = 0;
    }
}

static inline size_t terminal_vgaindex(const size_t x, const size_t y) {
    return (y - top_line) * VGA_WIDTH + x;
}

void terminal_putentryat_uint(const uint16_t entry, const size_t x, const size_t y) {
    lines[y][x] = entry;
    
    const size_t index = terminal_vgaindex(x, y);
    vga_buffer[index] = lines[y][x];
}

void terminal_putentryat(const char c, const uint8_t color, const size_t x, const size_t y) {
    uint16_t entry = vga_entry(c, color);
    terminal_putentryat_uint(entry, x, y);
}

void terminal_vgacursormove(const uint16_t pos) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0x00FF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos & 0xFF00) >> 8));
}

void terminal_cursormove(const size_t x, const size_t y) {
    uint16_t pos = terminal_vgaindex(x, y);
    terminal_vgacursormove(pos);
}

void terminal_updatecursor(void) {
    terminal_cursormove(column, row);
}

void terminal_updatevga(void) {
    memcpy(vga_buffer, lines[top_line], VGA_HEIGHT * VGA_WIDTH * sizeof(uint16_t));
    terminal_updatecursor();
}

void terminal_updateline(size_t line) {
    memcpy(vga_buffer + (line-top_line) * VGA_WIDTH, lines[line], VGA_WIDTH);
}

void terminal_moveleft(void) {
    if (column > 0) {
        column--; saved_column = column;
    } else if (row > 0) {
        row--;
        column = line_lengths[row]; saved_column = column;
    }

    terminal_updatecursor();
}

void terminal_moveright(void) {
    if (column < line_lengths[row]) {
        column++; saved_column = column;
    } else if (row < max_row) {
        column = 0; saved_column = 0;
        row++;
    }

    terminal_updatecursor();
}

void terminal_moveup(void) {
    if (row > 0) {
        row--;
        column = line_lengths[row] > saved_column ? saved_column : line_lengths[row];
    } else {
        column = 0; saved_column = 0;
    }

    terminal_updatecursor();
}

void terminal_movedown(void) {
    if (row < max_row) {
        row++;
        column = line_lengths[row] > saved_column ? saved_column : line_lengths[row];
    } else {
        column = line_lengths[row]; saved_column = column;
    }

    terminal_updatecursor();
}

void terminal_scrolldown(void) {
    if (top_line <= max_row - VGA_HEIGHT) {
        top_line++;
    }

    terminal_updatevga();
}

void terminal_scrollup(void) {
    if (top_line > 0) {
        top_line--;
    }

    terminal_updatevga();
}

void terminal_shiftdown(void) {
    if (++top_line == MAX_LINES - VGA_HEIGHT) {
        top_line--; row--;

        memcpy(lines, lines[1], (MAX_LINES - 1) * VGA_WIDTH * sizeof(uint16_t));
        memcpy(&line_lengths, &line_lengths[1], MAX_LINES * sizeof(size_t));

        for (size_t i = 0; i < VGA_WIDTH; i++) {
            lines[MAX_LINES-1][i] = vga_entry(' ', color);
        }

        line_lengths[MAX_LINES-1] = 0;
    }

    terminal_updatevga();
}

void terminal_newline(void) {
    column = 0; saved_column = 0;
    if (++row >= VGA_HEIGHT) {
        terminal_shiftdown();
    }

    max_row = row;
    line_lengths[row] = 0;

    terminal_updatecursor();
}

void terminal_erase(void) {
    if (column == 0 && row != 0) {
        row--; max_row--;
        column = line_lengths[row];

        terminal_scrollup();
    } else {
        column--; line_lengths[row]--;
    }

    saved_column = column;

    terminal_putentryat(' ', color, column, row);
    terminal_updatecursor();
}

void terminal_shiftright() {
    size_t i = row;
    uint16_t last_ch;

    while (line_lengths[i] == VGA_WIDTH && i < MAX_LINES) {
        last_ch = lines[i][VGA_WIDTH-1];

        memmove(&lines[i][1], &lines[i][0], (VGA_WIDTH - 1) * sizeof(uint16_t));
        terminal_updateline(i);

        terminal_putentryat_uint(last_ch, 0, i);
        i++;
    }

    line_lengths[i]++;
    memmove(&lines[i][column+1], &lines[i][column], (line_lengths[i] - column) * sizeof(uint16_t));
    terminal_updateline(i);
}

void terminal_putchar(char c) {
    terminal_shiftright();

    terminal_putentryat(c, color, column, row);
    if (++column == VGA_WIDTH) {
        terminal_newline();
    }
    saved_column = column;

    terminal_updatecursor();
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        const char ch = data[i];

        if (ch == '\r') {
            continue;
        } else if (ch == '\n') {
            terminal_newline();
        } else if (ch == '\b') {
            terminal_erase();
        } else {
            terminal_putchar(ch);
        }
    }
}

void terminal_writestring(const char* data) {
    terminal_write(data, strlen(data));
}

void terminal_writeint(int num) {
    char temp[16];
    char* tp = temp;

    if (num < 0) {
        terminal_putchar('-');
        num = -num;
    }

    do {
        int digit = num % 10;
        *tp++ = digit;
        num /= 10;
    } while (num > 0);

    while (tp > temp) {
        char ch = *--tp + '0';
        terminal_putchar(ch);
    }
}
