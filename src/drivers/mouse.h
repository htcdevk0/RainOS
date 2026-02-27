#ifndef RAINOS_MOUSE_H
#define RAINOS_MOUSE_H

#include "../kernel/types.h"

void mouse_init(void);
void mouse_handle_interrupt(void);

extern int mouse_x;
extern int mouse_y;
extern volatile int mouse_dirty;

#endif