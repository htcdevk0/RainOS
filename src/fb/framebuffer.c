#include "framebuffer.h"
#include "font.h"
#include "../drivers/keyboard.h"
#include <stdint-gcc.h>

Framebuffer g_framebuffer = {0};

uint32_t *fb_backbuffer = 0;

extern int mouse_y, mouse_x;
extern volatile int mouse_left_down;

extern volatile char keyboard_last_char;
extern int mouse_left_pressed_once;

static inline int fb_in_bounds(int x, int y)
{
    return (x >= 0 && y >= 0 &&
            (uint32_t)x < g_framebuffer.width &&
            (uint32_t)y < g_framebuffer.height);
}

static inline uint8_t color_r(uint32_t c) { return (uint8_t)((c >> 16) & 0xFF); }
static inline uint8_t color_g(uint32_t c) { return (uint8_t)((c >> 8) & 0xFF); }
static inline uint8_t color_b(uint32_t c) { return (uint8_t)(c & 0xFF); }

extern uint8_t _end;

void fb_init(uint64_t addr,
             uint32_t width,
             uint32_t height,
             uint32_t pitch,
             uint8_t bpp)
{
    g_framebuffer.address = (uintptr_t)addr;
    g_framebuffer.width = width;
    g_framebuffer.height = height;
    g_framebuffer.pitch = pitch;
    g_framebuffer.bpp = bpp;
    g_framebuffer.ready = 1;

    uintptr_t base = (uintptr_t)&_end;

    base = (base + 0xFFF) & ~0xFFF;

    fb_backbuffer = (uint32_t *)base;

    uint32_t total = width * height;
    for (uint32_t i = 0; i < total; i++)
        fb_backbuffer[i] = 0;
}

void fb_swap_buffers(void)
{
    if (!g_framebuffer.ready)
        return;
    if (g_framebuffer.bpp != 32)
        return;
    if (!fb_backbuffer)
        return;

    uint8_t *dst_base = (uint8_t *)g_framebuffer.address;
    uint32_t *src = fb_backbuffer;

    for (uint32_t y = 0; y < g_framebuffer.height; y++)
    {
        uint32_t *dst_row = (uint32_t *)(dst_base + y * g_framebuffer.pitch);
        uint32_t *src_row = src + y * g_framebuffer.width;

        for (uint32_t x = 0; x < g_framebuffer.width; x++)
        {
            dst_row[x] = src_row[x];
        }
    }
}

void fb_draw_icon(int x, int y, const FB_Icon *icon)
{
    if (!icon)
        return;

    for (int iy = 0; iy < icon->h; iy++)
    {
        for (int ix = 0; ix < icon->w; ix++)
        {

            uint32_t color = icon->pixels[iy * icon->w + ix];

            if (color != 0x00000000)
            {
                fb_put_pixel(x + ix, y + iy, color);
            }
        }
    }
}

void fb_draw_icon_scaled(int x, int y, const FB_Icon *icon, int scale)
{
    if (!icon || scale <= 0)
        return;

    for (int iy = 0; iy < icon->h; iy++)
    {
        for (int ix = 0; ix < icon->w; ix++)
        {
            uint32_t color = icon->pixels[iy * icon->w + ix];
            if (color == 0x00000000)
                continue;

            for (int sy = 0; sy < scale; sy++)
                for (int sx = 0; sx < scale; sx++)
                    fb_put_pixel(x + ix * scale + sx,
                                 y + iy * scale + sy,
                                 color);
        }
    }
}

void fb_draw_button(FB_Button *btn)
{
    if (!btn)
        return;

    extern int mouse_left_pressed_once;

    int hover =
        mouse_x >= btn->x &&
        mouse_x < btn->x + btn->w &&
        mouse_y >= btn->y &&
        mouse_y < btn->y + btn->h;

    uint32_t base = btn->color;

    if (hover)
        base = FB_RGB(210, 210, 210);

    if (hover && mouse_left_down)
        base = FB_RGB(175, 175, 175);

    fb_fill_rect(btn->x, btn->y, btn->w, btn->h, base);
    fb_draw_rect(btn->x, btn->y, btn->w, btn->h, FB_RGB(40, 40, 40));

    if (btn->icon)
    {
        int icon_x = btn->x + (btn->w - btn->icon->w) / 2;
        int icon_y = btn->y + 6;

        fb_draw_icon(icon_x, icon_y, btn->icon);

        if (btn->text)
        {
            int len = 0;
            while (btn->text[len])
                len++;

            int text_w = len > 0 ? len * (FONT_WIDTH + 1) - 1 : 0;

            int text_x = btn->x + (btn->w - text_w) / 2;
            int text_y = icon_y + btn->icon->h + 6;

            fb_draw_text(text_x, text_y, btn->text, FB_RGB(0, 0, 0));
        }
    }
    else
    {
        if (btn->text)
        {
            int len = 0;
            while (btn->text[len])
                len++;

            int text_w = len > 0 ? len * (FONT_WIDTH + 1) - 1 : 0;

            int text_x = btn->x + (btn->w - text_w) / 2;
            int text_y = btn->y + (btn->h - FONT_HEIGHT) / 2;

            fb_draw_text(text_x, text_y, btn->text, FB_RGB(0, 0, 0));
        }
    }

    if (hover && mouse_left_pressed_once && btn->on_click)
    {
        btn->on_click();
    }
}

void fb_put_pixel(int x, int y, uint32_t color)
{
    if (!g_framebuffer.ready)
        return;
    if (!fb_in_bounds(x, y))
        return;
    if (g_framebuffer.bpp != 32)
        return;
    if (!fb_backbuffer)
        return;

    fb_backbuffer[(uint32_t)y * g_framebuffer.width + (uint32_t)x] = color;
}

void fb_clear(uint32_t color)
{
    if (!g_framebuffer.ready)
        return;
    if (g_framebuffer.bpp != 32)
        return;
    if (!fb_backbuffer)
        return;

    uint32_t total = g_framebuffer.width * g_framebuffer.height;
    for (uint32_t i = 0; i < total; i++)
    {
        fb_backbuffer[i] = color;
    }
}

void fb_draw_hline(int x, int y, int w, uint32_t color)
{
    if (w <= 0)
        return;
    for (int i = 0; i < w; i++)
    {
        fb_put_pixel(x + i, y, color);
    }
}

void fb_draw_vline(int x, int y, int h, uint32_t color)
{
    if (h <= 0)
        return;
    for (int i = 0; i < h; i++)
    {
        fb_put_pixel(x, y + i, color);
    }
}

void fb_draw_rect(int x, int y, int w, int h, uint32_t color)
{
    if (w <= 0 || h <= 0)
        return;

    fb_draw_hline(x, y, w, color);
    fb_draw_hline(x, y + h - 1, w, color);
    fb_draw_vline(x, y, h, color);
    fb_draw_vline(x + w - 1, y, h, color);
}

void fb_fill_rect(int x, int y, int w, int h, uint32_t color)
{
    if (w <= 0 || h <= 0)
        return;

    for (int iy = 0; iy < h; iy++)
    {
        fb_draw_hline(x, y + iy, w, color);
    }
}

void fb_draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1)
    {
        fb_put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err << 1;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void fb_draw_char(int x, int y, char c, uint32_t color)
{
    unsigned char uc = (unsigned char)c;
    if (uc > 127)
        return;

    for (int row = 0; row < FONT_HEIGHT; row++)
    {
        uint8_t bits = font_data[uc][row];

        for (int bit = 0; bit < 8; bit++)
        {
            if (bits & (1 << (7 - bit)))
            {
                fb_put_pixel(x + bit, y + row, color);
            }
        }
    }
}

void fb_draw_text(int x, int y, const char *text, uint32_t color)
{
    int cursor_x = x;

    while (*text)
    {
        fb_draw_char(cursor_x, y, *text, color);
        cursor_x += FONT_WIDTH + 1;
        text++;
    }
}

void fb_draw_circle(int cx, int cy, int radius, uint32_t color)
{
    if (radius < 0)
        return;

    int x = radius;
    int y = 0;
    int err = 1 - x;

    while (x >= y)
    {
        fb_put_pixel(cx + x, cy + y, color);
        fb_put_pixel(cx + y, cy + x, color);
        fb_put_pixel(cx - y, cy + x, color);
        fb_put_pixel(cx - x, cy + y, color);
        fb_put_pixel(cx - x, cy - y, color);
        fb_put_pixel(cx - y, cy - x, color);
        fb_put_pixel(cx + y, cy - x, color);
        fb_put_pixel(cx + x, cy - y, color);

        y++;

        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void fb_fill_circle(int cx, int cy, int radius, uint32_t color)
{
    if (radius < 0)
        return;

    int x = radius;
    int y = 0;
    int err = 1 - x;

    while (x >= y)
    {
        fb_draw_hline(cx - x, cy + y, 2 * x + 1, color);
        fb_draw_hline(cx - x, cy - y, 2 * x + 1, color);
        fb_draw_hline(cx - y, cy + x, 2 * y + 1, color);
        fb_draw_hline(cx - y, cy - x, 2 * y + 1, color);

        y++;

        if (err < 0)
        {
            err += 2 * y + 1;
        }
        else
        {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

void fb_draw_gradient_vertical(uint32_t top_color, uint32_t bottom_color)
{
    if (!g_framebuffer.ready)
        return;
    if (g_framebuffer.height == 0)
        return;

    uint8_t tr = color_r(top_color);
    uint8_t tg = color_g(top_color);
    uint8_t tb = color_b(top_color);

    uint8_t br = color_r(bottom_color);
    uint8_t bg = color_g(bottom_color);
    uint8_t bb = color_b(bottom_color);

    uint32_t h = g_framebuffer.height;
    if (h == 1)
    {
        fb_clear(top_color);
        return;
    }

    for (uint32_t y = 0; y < h; y++)
    {
        uint32_t r = tr + ((int32_t)(br - tr) * (int32_t)y) / (int32_t)(h - 1);
        uint32_t g = tg + ((int32_t)(bg - tg) * (int32_t)y) / (int32_t)(h - 1);
        uint32_t b = tb + ((int32_t)(bb - tb) * (int32_t)y) / (int32_t)(h - 1);

        uint32_t color = FB_RGB(r, g, b);
        fb_draw_hline(0, (int)y, (int)g_framebuffer.width, color);
    }
}

void fb_draw_gradient_horizontal(uint32_t left_color, uint32_t right_color)
{
    if (!g_framebuffer.ready)
        return;
    if (g_framebuffer.width == 0)
        return;

    uint8_t lr = color_r(left_color);
    uint8_t lg = color_g(left_color);
    uint8_t lb = color_b(left_color);

    uint8_t rr = color_r(right_color);
    uint8_t rg = color_g(right_color);
    uint8_t rb = color_b(right_color);

    uint32_t w = g_framebuffer.width;
    if (w == 1)
    {
        fb_clear(left_color);
        return;
    }

    for (uint32_t x = 0; x < w; x++)
    {
        uint32_t r = lr + ((int32_t)(rr - lr) * (int32_t)x) / (int32_t)(w - 1);
        uint32_t g = lg + ((int32_t)(rg - lg) * (int32_t)x) / (int32_t)(w - 1);
        uint32_t b = lb + ((int32_t)(rb - lb) * (int32_t)x) / (int32_t)(w - 1);

        uint32_t color = FB_RGB(r, g, b);
        fb_draw_vline((int)x, 0, (int)g_framebuffer.height, color);
    }
}

void fb_textarea_handle_input(FB_TextArea *ta)
{
    if (!ta || !ta->buffer || ta->buffer_size <= 1)
        return;

    if (!ta->focused)
        return;

    char c;

    while ((c = keyboard_get_char()) != 0)
    {
        if (c == '\b')
        {
            if (ta->length > 0)
            {
                ta->length--;
                ta->buffer[ta->length] = 0;
            }
            continue;
        }

        if (c == '\n')
        {
            if (ta->on_enter)
                ta->on_enter(ta->buffer);
            continue;
        }

        if (c >= 32 && c <= 126)
        {
            if (ta->length < ta->buffer_size - 1)
            {
                ta->buffer[ta->length++] = c;
                ta->buffer[ta->length] = 0;
            }
        }
    }
}

void fb_draw_textarea(FB_TextArea *ta)
{
    if (!ta || !ta->buffer)
        return;

    if (mouse_left_pressed_once)
    {
        int hover =
            mouse_x >= ta->x &&
            mouse_x < ta->x + ta->w &&
            mouse_y >= ta->y &&
            mouse_y < ta->y + ta->h;

        ta->focused = hover ? 1 : 0;
    }

    fb_fill_rect(ta->x, ta->y, ta->w, ta->h, ta->bg_color);

    uint32_t border = ta->border_color;
    if (ta->focused)
        border = FB_RGB(70, 120, 255);

    fb_draw_rect(ta->x, ta->y, ta->w, ta->h, border);

    int text_x = ta->x + 6;
    int text_y = ta->y + (ta->h - FONT_HEIGHT) / 2;

    fb_draw_text(text_x, text_y, ta->buffer, ta->text_color);

    if (ta->focused)
    {
        int cursor_x = text_x + ta->length * (FONT_WIDTH + 1);
        int cursor_y = ta->y + 4;
        int cursor_h = ta->h - 8;

        if (cursor_x < ta->x + ta->w - 2)
            fb_draw_vline(cursor_x, cursor_y, cursor_h, FB_RGB(20, 20, 20));
    }
}