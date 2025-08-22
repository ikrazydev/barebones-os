#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "This is not a cross-compiler"
#endif // __linux__

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_LIGHT_GRAY = 7,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

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

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

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

const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0, '*',0, ' ',
};

bool scancode_states[128] = { false };

void welcome(void) {
    for (size_t i = 1; i <= 25; i++) {
        terminal_writestring("Hello, OS World ");
        terminal_writenumber(i);
        terminal_writestring(".\n");
    }
    
    terminal_writestring("Welcome to KrazShell\n");
    terminal_writestring("Time to use your keyboard.\n");
}

void kernel_main(void) {
    terminal_initialize();
    welcome();

    terminal_writestring("> ");

    while (1) {
        uint8_t sc = inb(0x60);
        if (sc & 0x80) {
            scancode_states[sc & 0x7F] = false;
            continue;
        }
        
        char c = scancode_to_ascii[sc];
        if (!c) continue;

        if (!scancode_states[sc]) {
            terminal_write(&c, 1);
            scancode_states[sc] = true;
        }
    }
}
