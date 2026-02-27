#ifndef DESKTOP_ICON_H
#define DESKTOP_ICON_H

#include "../fb/framebuffer.h"
#include "../modules/window.h"

typedef struct
{
    const char *label;

    int x;
    int y;
    int w;
    int h;

    FB_Icon *icon;
    FB_Window *window;
} FB_DesktopIcon;

void fb_desktop_icon_draw(FB_DesktopIcon *di);
void fb_desktop_icon_tick(FB_DesktopIcon *di);

#endif