#include "shell_icon.h"
#include "../fb/framebuffer.h"
#include <stdint-gcc.h>

#define C_TRANSPARENT 0x00000000
#define C_BG 0x111827
#define C_BORDER 0x374151
#define C_GREEN 0x22C55E
#define C_GREEN_DIM 0x16A34A

static uint32_t pixels[64 * 64];

static void put2x(int x, int y, uint32_t c)
{
    int X = x * 2;
    int Y = y * 2;

    pixels[(Y) * 64 + (X)] = c;
    pixels[(Y) * 64 + (X + 1)] = c;
    pixels[(Y + 1) * 64 + (X)] = c;
    pixels[(Y + 1) * 64 + (X + 1)] = c;
}

void shell_icon_init(void)
{
    for (int i = 0; i < 64 * 64; i++)
        pixels[i] = C_TRANSPARENT;

    for (int y = 4; y < 28; y++)
        for (int x = 4; x < 28; x++)
            put2x(x, y, C_BG);

    for (int x = 4; x < 28; x++)
    {
        put2x(x, 4, C_BORDER);
        put2x(x, 27, C_BORDER);
    }

    for (int y = 4; y < 28; y++)
    {
        put2x(4, y, C_BORDER);
        put2x(27, y, C_BORDER);
    }

    put2x(10, 13, C_GREEN_DIM);
    put2x(11, 14, C_GREEN_DIM);
    put2x(10, 15, C_GREEN_DIM);

    put2x(9, 12, C_GREEN);
    put2x(10, 13, C_GREEN);
    put2x(11, 14, C_GREEN);
    put2x(10, 15, C_GREEN);
    put2x(9, 16, C_GREEN);

    for (int x = 15; x < 22; x++)
        put2x(x, 18, C_GREEN);
}

FB_Icon icon_shell_app = {
    .w = 64,
    .h = 64,
    .pixels = pixels};