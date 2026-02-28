#ifndef CURSOR_H
#define CURSOR_H

#include <stdint-gcc.h>

extern volatile int g_cursor_pointer;

void draw_cursor(int x, int y);

#endif