#ifndef FONT_H
#define FONT_H
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#include <stdint-gcc.h>


#define FONT_WIDTH 8
#define FONT_HEIGHT 16

extern const uint8_t font_data[128][FONT_HEIGHT];

#endif