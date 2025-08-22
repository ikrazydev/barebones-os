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
    for (size_t i = 0; i <= 100; i += 1) {
        terminal_writestring("Booting... ");
        terminal_writenumber(i);
        terminal_writestring("%\n");
    }

    terminal_writestring("Welcome to KrazShell!\n");
    terminal_writestring("Time to use your keyboard.\n");
}

void read_ext_key(void) {
    uint8_t sc = inb(0x60);

    if (sc & 0x80) {
        sc_ext_states[0x7F & sc] = false;
        return;
    }

    if (!sc_ext_states[sc]) {
        sc_ext_states[sc] = true;

        if (!is_shift_down() && sc == SC_EXT_LFCUR) {
            terminal_moveleft();
        } else if (!is_shift_down() && sc == SC_EXT_RGCUR) {
            terminal_moveright();
        } else if (!is_shift_down() && sc == SC_EXT_UPCUR) {
            terminal_moveup();
        } else if (!is_shift_down() && sc == SC_EXT_DWCUR) {
            terminal_movedown();
        }

        if (is_shift_down() && sc == SC_EXT_DWCUR) {
            terminal_scrolldown();
        } else if (is_shift_down() && sc == SC_EXT_UPCUR) {
            terminal_scrollup();
        }
    }
}

void read_key(void) {
    uint8_t sc = inb(0x60);
    if (sc == 0xE0) {
        read_ext_key();
        return;
    }

    if (sc & 0x80) {
        sc_states[sc & 0x7F] = false;
        return;
    }

    bool is_upper = is_shift_down() ^ sc_states[SC_CAPS];

    char c = is_upper ? sc_usqwerty_upper[sc] : sc_usqwerty_lower[sc];
    if (!sc_states[sc]) {
        sc_states[sc] = true;

        if (!c) return;
        terminal_write(&c, 1);
    }
}

void kernel_main(void) {
    terminal_initialize();
    welcome();

    terminal_writestring("> ");

    while (true) {
        read_key();
    }
}
