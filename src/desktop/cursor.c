#include <stdint-gcc.h>
#include "../fb/framebuffer.h"

volatile int g_cursor_pointer = 0;

void draw_cursor(int x, int y)
{
    uint32_t black = FB_RGB(0, 0, 0);
    uint32_t white = FB_RGB(255, 255, 255);
    uint32_t shadow = FB_RGB(0, 0, 0);
    uint32_t aa_gray = FB_RGB(192, 192, 192);

    if (!g_cursor_pointer)
    {

        const int H = 24;
        const int OUTER_MAX = 14;
        const int INNER_MAX = 12;

        for (int i = 0; i < H; i++)
        {
            int w = (i * (OUTER_MAX + 3)) / (H - 1);
            for (int j = 0; j <= w; j++)
                fb_put_pixel(x + j + 2, y + i + 2, shadow);
        }

        for (int i = 0; i < H; i++)
        {
            int w = (i * (OUTER_MAX + 2)) / (H - 1) + 1;
            for (int j = 0; j <= w; j++)
                fb_put_pixel(x + j + 1, y + i + 1, shadow);
        }

        for (int i = 0; i < H; i++)
        {
            int w = (i * OUTER_MAX) / (H - 1);
            for (int j = 0; j <= w; j++)
                fb_put_pixel(x + j, y + i, white);
        }

        for (int i = 1; i < H - 1; i++)
        {
            int w = (i * INNER_MAX) / (H - 1);
            for (int j = 1; j <= w; j++)
                fb_put_pixel(x + j, y + i, black);
        }

        for (int i = 0; i < H; i++)
        {
            int w = (i * OUTER_MAX) / (H - 1);
            fb_put_pixel(x + w + 1, y + i, aa_gray);
        }
    }
    else
    {

        const int H = 24;
        const int OUTER_MAX = 14;

        for (int i = 0; i < H; i++)
        {
            int w = (i * (OUTER_MAX + 3)) / (H - 1);
            for (int j = 0; j <= w; j++)
                fb_put_pixel(x + j + 2, y + i + 2, shadow);
        }

        for (int i = 0; i < H; i++)
        {
            int w = (i * (OUTER_MAX + 2)) / (H - 1) + 1;
            for (int j = 0; j <= w; j++)
                fb_put_pixel(x + j + 1, y + i + 1, shadow);
        }

        for (int i = 0; i < H; i++)
        {
            int w = (i * OUTER_MAX) / (H - 1);
            for (int j = 0; j <= w; j++)
                fb_put_pixel(x + j, y + i, white);
        }
    }
}