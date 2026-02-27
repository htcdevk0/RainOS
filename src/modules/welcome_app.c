#include "welcome_app.h"

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

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

static int fb_strlen_local(const char *s)
{
    int n = 0;
    while (s && s[n])
        n++;
    return n;
}

static int g_icon_x = 40;
static int g_icon_y = 90;
static int g_icon_w = 80;
static int g_icon_h = 90;

static FB_Window g_win = {
    .title = "Welcome",
    .x = 140,
    .y = 110,
    .w = 520,
    .h = 360,
    .created = 0,
    .open = 0};

void welcome_app_init(void)
{

    g_win.created = 0;
    g_win.open = 0;
}

void welcome_app_tick(void)
{
    int now = (mouse_left_down != 0);
    g_left_pressed_once = (now && !g_prev_left);
    g_prev_left = now;

    if (!g_left_pressed_once)
        return;

    if (point_in_rect(mouse_x, mouse_y, g_icon_x, g_icon_y, g_icon_w, g_icon_h))
    {
        if (!g_win.created)
            fb_createWindow(&g_win);
        else
            fb_openWindow(&g_win);

        mouse_dirty = 1;
    }
}

void welcome_app_open(void)
{
    if (!g_win.created)
        fb_createWindow(&g_win);
    else
        fb_openWindow(&g_win);
}

void welcome_app_draw_desktop(void)
{
    int box_x = g_icon_x;
    int box_y = g_icon_y;

    int hover = point_in_rect(mouse_x, mouse_y,
                              g_icon_x, g_icon_y,
                              g_icon_w, g_icon_h);

    if (hover)
    {
        fb_fill_rect(box_x, box_y,
                     g_icon_w, g_icon_h,
                     FB_RGB(255, 255, 255));

        fb_draw_rect(box_x, box_y,
                     g_icon_w, g_icon_h,
                     FB_RGB(120, 130, 145));
    }

    int icon_area_h = g_icon_h - FONT_HEIGHT - 6;

    int scale = 2;

    int scaled_w = icon_generic_app.w * scale;
    int scaled_h = icon_generic_app.h * scale;

    int ix = box_x + (g_icon_w - scaled_w) / 2;
    int iy = box_y + (icon_area_h - scaled_h) / 2;

    fb_draw_icon_scaled(ix, iy, &icon_generic_app, scale);

    const char *label = "Welcome";
    int len = fb_strlen_local(label);
    int tw = len * (FONT_WIDTH + 1) - 1;

    int tx = box_x + (g_icon_w - tw) / 2;
    int ty = box_y + g_icon_h - FONT_HEIGHT - 2;

    fb_draw_text(tx + 1, ty + 1, label, FB_RGB(0, 0, 0));

    fb_draw_text(tx, ty, label, FB_RGB(245, 245, 245));
}

void welcome_app_draw_windows(void)
{

    fb_drawWindow(&g_win);

    if (!g_win.open)
        return;

    fb_window_content(&g_win);

    fb_window_draw_text(&g_win, 14, 14, "Welcome to RainOS.", FB_RGB(25, 25, 28));
    fb_window_draw_text(&g_win, 14, 34, "This is your first app icon + window.", FB_RGB(45, 45, 50));

    fb_window_fill_rect(&g_win, 14, 62, 260, 70, FB_RGB(230, 235, 242));
    fb_window_draw_rect(&g_win, 14, 62, 260, 70, FB_RGB(90, 96, 104));
    fb_window_draw_text(&g_win, 22, 74, "> hello world", FB_RGB(20, 80, 30));

    FB_Button ok = (FB_Button){0};
    ok.x = 14;
    ok.y = 150;
    ok.w = 120;
    ok.h = 30;
    ok.text = "OK";
    ok.color = FB_RGB(225, 225, 225);
    ok.on_click = 0;
    ok.icon = 0;
    fb_window_draw_button(&g_win, &ok);

    fb_window_end_content(&g_win);
}