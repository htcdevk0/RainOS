#include "desktop.h"
#include "../drivers/mouse.h"
#include "../fb/font.h"
#include "../modules/window.h"

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_dirty;
extern volatile int g_cursor_pointer;

#define MAX_DESKTOP_ICONS 32

static FB_DesktopIcon *g_icons[MAX_DESKTOP_ICONS];
static int g_icon_count = 0;

static FB_DesktopIcon *g_drag_icon = 0;
static FB_DesktopIcon *g_pressed_icon = 0;

static int g_prev_left_down = 0;
static int g_mouse_left_pressed_once_desktop = 0;
static int g_mouse_left_released_once_desktop = 0;

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

static int clampi(int v, int minv, int maxv)
{
    if (v < minv)
        return minv;
    if (v > maxv)
        return maxv;
    return v;
}

void fb_desktop_input_begin_frame(void)
{
    int now = (mouse_left_down != 0);
    g_mouse_left_pressed_once_desktop = (now && !g_prev_left_down);
    g_mouse_left_released_once_desktop = (!now && g_prev_left_down);
    g_prev_left_down = now;
}

void fb_desktop_register_icon(FB_DesktopIcon *icon)
{
    if (!icon)
        return;
    if (g_icon_count >= MAX_DESKTOP_ICONS)
        return;

    g_icons[g_icon_count++] = icon;
}

void fb_desktop_tick(void)
{
    if (fb_ui_mouse_captured())
    {

        g_drag_icon = 0;
        g_pressed_icon = 0;
        return;
    }
    for (int i = 0; i < g_icon_count; i++)
    {
        FB_DesktopIcon *di = g_icons[i];
        int inside = point_in_rect(mouse_x, mouse_y, di->x, di->y, di->w, di->h);

        if (g_mouse_left_pressed_once_desktop && inside)
        {
            g_pressed_icon = di;
            g_drag_icon = di;

            di->drag_off_x = mouse_x - di->x;
            di->drag_off_y = mouse_y - di->y;

            di->press_x = mouse_x;
            di->press_y = mouse_y;
            di->moved = 0;
        }
    }

    if (g_drag_icon && mouse_left_down)
    {
        FB_DesktopIcon *di = g_drag_icon;

        int dx = mouse_x - di->press_x;
        int dy = mouse_y - di->press_y;
        if (dx < 0)
            dx = -dx;
        if (dy < 0)
            dy = -dy;

        if (dx >= 3 || dy >= 3)
            di->moved = 1;

        di->x = clampi(mouse_x - di->drag_off_x,
                       0,
                       (int)g_framebuffer.width - di->w);

        di->y = clampi(mouse_y - di->drag_off_y,
                       0,
                       (int)g_framebuffer.height - di->h);

        mouse_dirty = 1;
    }

    if (g_mouse_left_released_once_desktop)
    {
        if (g_pressed_icon && !g_pressed_icon->moved)
        {
            if (g_pressed_icon->window)
            {
                if (!g_pressed_icon->window->created)
                    fb_createWindow(g_pressed_icon->window);
                else
                    fb_openWindow(g_pressed_icon->window);
            }
        }

        g_drag_icon = 0;
        g_pressed_icon = 0;

        mouse_dirty = 1;
    }
}

void fb_desktop_draw(void)
{
    for (int i = 0; i < g_icon_count; i++)
    {
        FB_DesktopIcon *di = g_icons[i];

        int hover = point_in_rect(mouse_x, mouse_y, di->x, di->y, di->w, di->h);

        if (hover)
        {
            g_cursor_pointer = 1;
            fb_fill_rect(di->x, di->y, di->w, di->h,
                         FB_RGB(255, 255, 255));
            fb_draw_rect(di->x, di->y, di->w, di->h,
                         FB_RGB(120, 130, 145));
        }

        if (di->icon)
        {
            int icon_area_h = di->h - FONT_HEIGHT - 8;

            int ix = di->x + (di->w - di->icon->w) / 2;
            int iy = di->y + (icon_area_h - di->icon->h) / 2;

            fb_draw_icon(ix, iy, di->icon);
        }

        if (di->label)
        {
            int len = 0;
            while (di->label[len])
                len++;

            int tw = len * (FONT_WIDTH + 1) - 1;
            int tx = di->x + (di->w - tw) / 2;
            int ty = di->y + di->h - FONT_HEIGHT - 2;

            fb_draw_text(tx + 1, ty + 1,
                         di->label,
                         FB_RGB(0, 0, 0));

            fb_draw_text(tx, ty,
                         di->label,
                         FB_RGB(245, 245, 245));
        }
    }
}