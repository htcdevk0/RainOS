#include "patches_app.h"

#include "../fb/framebuffer.h"
#include "../fb/font.h"
#include "../drivers/mouse.h"
#include "../icons/generic_app.h"
#include "window.h"

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_dirty;

static int g_prev_left = 0;
static int g_left_pressed_once = 0;

static int g_icon_x = 240;
static int g_icon_y = 90;
static int g_icon_w = 80;
static int g_icon_h = 90;

static FB_Window g_win = {
    .title = "Patches",
    .x = 220,
    .y = 160,
    .w = 500,
    .h = 260,
    .created = 0,
    .open = 0
};

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

void patches_app_init(void)
{
    g_win.created = 0;
    g_win.open = 0;
}

void patches_app_tick(void)
{
    int now = (mouse_left_down != 0);
    g_left_pressed_once = (now && !g_prev_left);
    g_prev_left = now;

    if (!g_left_pressed_once)
        return;

    if (point_in_rect(mouse_x, mouse_y,
                      g_icon_x, g_icon_y,
                      g_icon_w, g_icon_h))
    {
        if (!g_win.created)
            fb_createWindow(&g_win);
        else
            fb_openWindow(&g_win);

        mouse_dirty = 1;
    }
}

void patches_app_draw_desktop(void)
{
    int hover = point_in_rect(mouse_x, mouse_y,
                              g_icon_x, g_icon_y,
                              g_icon_w, g_icon_h);

    if (hover)
    {
        fb_fill_rect(g_icon_x, g_icon_y,
                     g_icon_w, g_icon_h,
                     FB_RGB(255, 255, 255));

        fb_draw_rect(g_icon_x, g_icon_y,
                     g_icon_w, g_icon_h,
                     FB_RGB(120, 130, 145));
    }

    int scale = 2;

    int scaled_w = icon_generic_app.w * scale;
    int scaled_h = icon_generic_app.h * scale;

    int ix = g_icon_x + (g_icon_w - scaled_w) / 2;
    int iy = g_icon_y + ((g_icon_h - FONT_HEIGHT - 6) - scaled_h) / 2;

    fb_draw_icon_scaled(ix, iy, &icon_generic_app, scale);

    const char *label = "Patches";

    int len = 0;
    while (label[len]) len++;

    int tw = len * (FONT_WIDTH + 1) - 1;

    int tx = g_icon_x + (g_icon_w - tw) / 2;
    int ty = g_icon_y + g_icon_h - FONT_HEIGHT - 2;

    fb_draw_text(tx + 1, ty + 1, label, FB_RGB(0, 0, 0));
    fb_draw_text(tx, ty, label, FB_RGB(245, 245, 245));
}

void patches_app_draw_windows(void)
{
    fb_drawWindow(&g_win);

    if (!g_win.open)
        return;

    fb_window_content(&g_win);

    fb_window_draw_text(&g_win, 16, 16,
        "RainOS Patch Notes",
        FB_RGB(25, 25, 28));

    fb_window_draw_text(&g_win, 16, 46,
        "Patches App Added.",
        FB_RGB(45, 45, 50));

    fb_window_draw_text(&g_win, 16, 66,
        "Github Repository Released.",
        FB_RGB(45, 45, 50));

    fb_window_draw_text(&g_win, 16, 96,
        "Version: 1.1.0",
        FB_RGB(20, 80, 30));

    fb_window_end_content(&g_win);
}