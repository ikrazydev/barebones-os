#ifndef TERMINAL_H
#define TERMINAL_H

void terminal_initialize(void);

void terminal_moveleft(void);
void terminal_moveright(void);
void terminal_moveup(void);
void terminal_movedown(void);

void terminal_scrolldown(void);
void terminal_scrollup(void);

void terminal_erase(void);

void terminal_putchar(const char c);
void terminal_write(const char* data, const size_t size);
void terminal_writestring(const char* data);
void terminal_writeint(int num);

void terminal_newline(void);

#endif
