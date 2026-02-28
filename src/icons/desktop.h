#ifndef FB_DESKTOP_H
#define FB_DESKTOP_H

#include "../fb/framebuffer.h"
#include "../modules/window.h"

typedef struct FB_DesktopIcon
{
    int x, y;
    int w, h;

    const char *label;
    FB_Icon *icon;
    FB_Window *window;

        int drag_off_x;
    int drag_off_y;

    int press_x;
    int press_y;

    int moved;
} FB_DesktopIcon;

void fb_desktop_input_begin_frame(void);
void fb_desktop_register_icon(FB_DesktopIcon *icon);

void fb_desktop_tick(void);
void fb_desktop_draw(void);

#endif