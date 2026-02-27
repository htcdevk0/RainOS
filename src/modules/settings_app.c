#include "settings_app.h"

#include "../fb/framebuffer.h"
#include "../fb/font.h"
#include "../drivers/mouse.h"
#include "../icons/generic_app.h"
#include "window.h"

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_dirty;

WallpaperState g_wallpaper = {WALLPAPER_DEFAULT};

static int g_prev_left = 0;
static int g_left_pressed_once = 0;

static int g_icon_x = 440;
static int g_icon_y = 90;
static int g_icon_w = 80;
static int g_icon_h = 90;

static FB_Window g_win = {
    .title = "Settings",
    .x = 300,
    .y = 160,
    .w = 600,
    .h = 420,
    .created = 0,
    .open = 0};

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

void settings_app_init(void)
{
    g_win.created = 0;
    g_win.open = 0;
}

void settings_app_tick(void)
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

static void set_wallpaper_default(void)
{
    g_wallpaper.type = WALLPAPER_DEFAULT;
    mouse_dirty = 1;
}

static void set_wallpaper_dark(void)
{
    g_wallpaper.type = WALLPAPER_DARK;
    mouse_dirty = 1;
}

static void set_wallpaper_flat(void)
{
    g_wallpaper.type = WALLPAPER_FLAT;
    mouse_dirty = 1;
}

void settings_app_draw_desktop(void)
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

    const char *label = "Settings";

    int len = 0;
    while (label[len])
        len++;

    int tw = len * (FONT_WIDTH + 1) - 1;

    int tx = g_icon_x + (g_icon_w - tw) / 2;
    int ty = g_icon_y + g_icon_h - FONT_HEIGHT - 2;

    fb_draw_text(tx + 1, ty + 1, label, FB_RGB(0, 0, 0));
    fb_draw_text(tx, ty, label, FB_RGB(245, 245, 245));
}

static void draw_wallpaper_preview(int x, int y, int w, int h, WallpaperType type)
{
    fb_draw_rect(x, y, w, h, FB_RGB(80, 80, 80));

    if (type == WALLPAPER_DEFAULT)
    {
        for (int i = 0; i < h; i++)
        {
            uint32_t r = 140 + (i * 75) / h;
            uint32_t g = 185 + (i * 47) / h;
            uint32_t b = 235 - (i * 15) / h;
            fb_fill_rect(x + 1, y + 1 + i, w - 2, 1, FB_RGB(r, g, b));
        }
    }
    else if (type == WALLPAPER_DARK)
    {
        for (int i = 0; i < h; i++)
        {
            uint32_t v = 30 + (i * 40) / h;
            fb_fill_rect(x + 1, y + 1 + i, w - 2, 1, FB_RGB(v, v, v + 10));
        }
    }
    else if (type == WALLPAPER_FLAT)
    {
        fb_fill_rect(x + 1, y + 1, w - 2, h - 2, FB_RGB(160, 160, 160));
    }
}

void settings_app_draw_windows(void)
{
    fb_drawWindow(&g_win);

    if (!g_win.open)
        return;

    fb_window_content(&g_win);

    fb_window_draw_text(&g_win, 20, 16,
                        "Wallpaper Settings",
                        FB_RGB(25, 25, 28));

    int base_y = 50;

    draw_wallpaper_preview(g_win.content_x + 20,
                           g_win.content_y + base_y,
                           140, 90,
                           WALLPAPER_DEFAULT);

    draw_wallpaper_preview(g_win.content_x + 220,
                           g_win.content_y + base_y,
                           140, 90,
                           WALLPAPER_DARK);

    draw_wallpaper_preview(g_win.content_x + 420,
                           g_win.content_y + base_y,
                           140, 90,
                           WALLPAPER_FLAT);

    FB_Button b1 = {0};
    b1.x = 20;
    b1.y = base_y + 110;
    b1.w = 140;
    b1.h = 28;
    b1.text = "Default";
    b1.color = FB_RGB(225, 225, 225);
    b1.on_click = set_wallpaper_default;
    fb_window_draw_button(&g_win, &b1);

    FB_Button b2 = {0};
    b2.x = 220;
    b2.y = base_y + 110;
    b2.w = 140;
    b2.h = 28;
    b2.text = "Dark";
    b2.color = FB_RGB(225, 225, 225);
    b2.on_click = set_wallpaper_dark;
    fb_window_draw_button(&g_win, &b2);

    FB_Button b3 = {0};
    b3.x = 420;
    b3.y = base_y + 110;
    b3.w = 140;
    b3.h = 28;
    b3.text = "Flat Gray";
    b3.color = FB_RGB(225, 225, 225);
    b3.on_click = set_wallpaper_flat;
    fb_window_draw_button(&g_win, &b3);

    fb_window_end_content(&g_win);
}