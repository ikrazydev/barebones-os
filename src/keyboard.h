#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>

#define SC_MAX 0x7F

#define SC_LCTRL 0x1D
#define SC_LALT 0x38
#define SC_LSHIFT 0x2A
#define SC_RSHIFT 0x36
#define SC_CAPS 0x3A

#define SC_EXT_RCTRL 0x1D
#define SC_EXT_RALT 0x38

#define SC_EXT_UPCUR 0x48
#define SC_EXT_LFCUR 0x4B
#define SC_EXT_RGCUR 0x4D
#define SC_EXT_DWCUR 0x50

extern const char sc_usqwerty_lower[SC_MAX];
extern const char sc_usqwerty_upper[SC_MAX];

extern bool sc_states[SC_MAX];
extern bool sc_ext_states[SC_MAX];

bool is_ctrl_down(void);
bool is_alt_down(void);
bool is_shift_down(void);

#endif
