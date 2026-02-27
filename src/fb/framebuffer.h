#ifndef RAINOS_FRAMEBUFFER_H
#define RAINOS_FRAMEBUFFER_H

#include "../kernel/types.h"
#include <stdint-gcc.h>

typedef struct
{
    uintptr_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    int ready;
} Framebuffer;

typedef struct
{
    int w;
    int h;
    const uint32_t *pixels;
} FB_Icon;

typedef void (*button_callback_t)(void);
typedef struct
{
    int x, y;
    int w, h;
    const char *text;
    uint32_t color;
    button_callback_t on_click;
    const FB_Icon *icon;
} FB_Button;

extern Framebuffer g_framebuffer;

extern uint32_t *fb_backbuffer;

#define FB_RGB(r, g, b) ((uint32_t)(((r) << 16) | ((g) << 8) | (b)))

void fb_init(uint64_t addr,
             uint32_t width,
             uint32_t height,
             uint32_t pitch,
             uint8_t bpp);

void fb_clear(uint32_t color);
void fb_put_pixel(int x, int y, uint32_t color);

void fb_swap_buffers(void);

void fb_draw_hline(int x, int y, int w, uint32_t color);
void fb_draw_vline(int x, int y, int h, uint32_t color);
void fb_draw_rect(int x, int y, int w, int h, uint32_t color);
void fb_fill_rect(int x, int y, int w, int h, uint32_t color);
void fb_draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void fb_draw_circle(int cx, int cy, int radius, uint32_t color);
void fb_fill_circle(int cx, int cy, int radius, uint32_t color);

void fb_draw_gradient_vertical(uint32_t top_color, uint32_t bottom_color);
void fb_draw_gradient_horizontal(uint32_t left_color, uint32_t right_color);

void fb_draw_text(int x, int y, const char *text, uint32_t color);
void fb_draw_char(int x, int y, char c, uint32_t color);

void fb_draw_button(FB_Button *btn);

void fb_draw_icon(int x, int y, const FB_Icon *icon);

void fb_draw_icon_scaled(int x, int y, const FB_Icon *icon, int scale);

#endif