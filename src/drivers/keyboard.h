#ifndef RAINOS_KEYBOARD_H
#define RAINOS_KEYBOARD_H

char keyboard_get_char(void);

void keyboard_init(void);
void keyboard_handle_interrupt(void);

#endif