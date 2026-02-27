#include "desktop.h"
#include "../drivers/mouse.h"
#include "../fb/font.h"

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_dirty;

static int prev_left = 0;

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

void fb_desktop_icon_tick(FB_DesktopIcon *di)
{
    int now = mouse_left_down;
    int pressed_once = (now && !prev_left);
    prev_left = now;

    if (!pressed_once)
        return;

    if (point_in_rect(mouse_x, mouse_y, di->x, di->y, di->w, di->h))
    {
        if (!di->window->created)
            fb_createWindow(di->window);
        else
            fb_openWindow(di->window);

        mouse_dirty = 1;
    }
}

void fb_desktop_icon_draw(FB_DesktopIcon *di)
{
    int hover = point_in_rect(mouse_x, mouse_y,
                              di->x, di->y,
                              di->w, di->h);

    if (hover)
    {
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

        fb_draw_text(tx + 1, ty + 1, di->label, FB_RGB(0, 0, 0));

        fb_draw_text(tx, ty, di->label, FB_RGB(245, 245, 245));
    }
}