#include "showcase_app.h"

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

static int g_icon_x = 340;
static int g_icon_y = 90;
static int g_icon_w = 80;
static int g_icon_h = 90;

static FB_Window g_win = {
    .title = "Showcase",
    .x = 260,
    .y = 140,
    .w = 620,
    .h = 420,
    .created = 0,
    .open = 0};

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

void showcase_app_init(void)
{
    g_win.created = 0;
    g_win.open = 0;
}

void showcase_app_tick(void)
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

void showcase_app_draw_desktop(void)
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

    const char *label = "Showcase";

    int len = 0;
    while (label[len])
        len++;

    int tw = len * (FONT_WIDTH + 1) - 1;

    int tx = g_icon_x + (g_icon_w - tw) / 2;
    int ty = g_icon_y + g_icon_h - FONT_HEIGHT - 2;

    fb_draw_text(tx + 1, ty + 1, label, FB_RGB(0, 0, 0));
    fb_draw_text(tx, ty, label, FB_RGB(245, 245, 245));
}

void showcase_app_draw_windows(void)
{
    fb_drawWindow(&g_win);

    if (!g_win.open)
        return;

    fb_window_content(&g_win);

    fb_window_draw_text(&g_win, 16, 14,
                        "Window System Showcase",
                        FB_RGB(25, 25, 28));

    fb_window_fill_rect(&g_win, 16, 40, 120, 60,
                        FB_RGB(200, 220, 255));
    fb_window_draw_rect(&g_win, 16, 40, 120, 60,
                        FB_RGB(60, 90, 140));

    fb_window_draw_text(&g_win, 22, 110,
                        "Rectangles + Borders",
                        FB_RGB(45, 45, 50));

    fb_window_fill_rect(&g_win, 180, 40, 120, 120,
                        FB_RGB(240, 240, 240));

    fb_draw_circle(
        g_win.content_x + 240,
        g_win.content_y + 100,
        40,
        FB_RGB(200, 80, 80));

    fb_fill_circle(
        g_win.content_x + 240,
        g_win.content_y + 100,
        25,
        FB_RGB(80, 160, 220));

    fb_window_draw_text(&g_win, 180, 170,
                        "Circles",
                        FB_RGB(45, 45, 50));

    for (int i = 0; i < 120; i++)
    {
        uint32_t r = 80 + (i * 120) / 120;
        uint32_t g = 100;
        uint32_t b = 200 - (i * 120) / 120;

        fb_window_fill_rect(&g_win,
                            350,
                            40 + i,
                            180,
                            1,
                            FB_RGB(r, g, b));
    }

    fb_window_draw_rect(&g_win, 350, 40, 180, 120,
                        FB_RGB(60, 60, 60));

    fb_window_draw_text(&g_win, 350, 170,
                        "Gradient Rendering",
                        FB_RGB(45, 45, 50));

    FB_Button btn = {0};
    btn.x = 16;
    btn.y = 210;
    btn.w = 220;
    btn.h = 36;
    btn.text = "Hover or Click Me";
    btn.color = FB_RGB(225, 225, 225);
    btn.on_click = 0;

    fb_window_draw_button(&g_win, &btn);

    fb_window_draw_text(&g_win, 16, 270,
                        "This button demonstrates hover + click states.",
                        FB_RGB(50, 50, 55));

    fb_window_end_content(&g_win);
}