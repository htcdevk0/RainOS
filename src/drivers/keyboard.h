#ifndef KEYBOARD_H
#define KEYBOARD_H

extern volatile char keyboard_last_char;

void keyboard_init(void);

void keyboard_handle_interrupt(void);

#endif