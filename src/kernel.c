#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "terminal.h"
#include "asm.h"
#include "keyboard.h"

#ifdef __linux__
#error "This is not a cross-compiler"
#endif // __linux__

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

    while (true) {
        uint8_t sc = inb(0x60);
        if (sc & 0x80) {
            scancode_states[sc & 0x7F] = false;
            continue;
        }
        
        char c = sc_usqwerty_lower[sc];
        if (!c) continue;

        if (!scancode_states[sc]) {
            terminal_write(&c, 1);
            scancode_states[sc] = true;
        }
    }
}
