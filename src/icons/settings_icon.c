#include "settings_icon.h"
#include "../fb/framebuffer.h"
#include <stdint-gcc.h>

#define C_TRANSPARENT 0x00000000
#define C_GEAR_LIGHT 0xD1D5DB
#define C_GEAR 0x9CA3AF
#define C_GEAR_DARK 0x4B5563

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

void settings_icon_init(void)
{
    for (int i = 0; i < 64 * 64; i++)
        pixels[i] = C_TRANSPARENT;

    int cx = 16;
    int cy = 16;

    for (int y = 4; y < 28; y++)
    {
        for (int x = 4; x < 28; x++)
        {
            int dx = x - cx;
            int dy = y - cy;
            int dist2 = dx * dx + dy * dy;

            if (dist2 >= 70 && dist2 <= 120)
            {
                uint32_t color;

                if (dx > 0 && dy > 0)
                    color = C_GEAR_DARK;
                else if (dx < 0 && dy < 0)
                    color = C_GEAR_LIGHT;
                else
                    color = C_GEAR;

                put2x(x, y, color);
            }
        }
    }

    int teeth[8][2] = {
        {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}};

    for (int t = 0; t < 8; t++)
    {
        int tx = teeth[t][0];
        int ty = teeth[t][1];

        for (int i = 9; i <= 12; i++)
        {
            int x = cx + tx * i;
            int y = cy + ty * i;

            if (x >= 0 && x < 32 && y >= 0 && y < 32)
                put2x(x, y, C_GEAR);
        }
    }

    for (int y = 10; y < 22; y++)
    {
        for (int x = 10; x < 22; x++)
        {
            int dx = x - cx;
            int dy = y - cy;
            int dist2 = dx * dx + dy * dy;

            if (dist2 <= 25)
                put2x(x, y, C_TRANSPARENT);
        }
    }
}

FB_Icon icon_settings_app = {
    .w = 64,
    .h = 64,
    .pixels = pixels};