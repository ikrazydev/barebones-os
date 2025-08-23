#include "keyboard.h"

const char sc_usqwerty_lower[SC_MAX] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0, '*',0, ' ',
};

const char sc_usqwerty_upper[SC_MAX] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','Y','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0, '*',0, ' ',
};

bool sc_states[SC_MAX] = { false };
bool sc_ext_states[SC_MAX] = { false };

bool is_ctrl_down(void) {
    return sc_states[SC_LCTRL] || sc_ext_states[SC_EXT_RCTRL];
}

bool is_alt_down(void) {
    return sc_states[SC_LALT] || sc_ext_states[SC_EXT_RALT];
}

bool is_shift_down(void) {
    return sc_states[SC_LSHIFT] || sc_states[SC_RSHIFT];
}
