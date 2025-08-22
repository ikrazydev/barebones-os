#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#define SC_LSHIFT 0x2A
#define SC_RSHIFT 0x36
#define SC_CAPS 0x3A

#define SC_EXT_UPCUR 0x48
#define SC_EXT_LFCUR 0x4B
#define SC_EXT_RGCUR 0x4D
#define SC_EXT_DWCUR 0x50

extern const char sc_usqwerty_lower[128];
extern const char sc_usqwerty_upper[128];

extern bool sc_states[128];
extern bool sc_ext_states[128];

bool is_shift_down(void);

#endif
