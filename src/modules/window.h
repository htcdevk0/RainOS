#ifndef FB_WINDOW_H
#define FB_WINDOW_H

#include <stdint-gcc.h>
#include "framebuffer.h"

typedef struct fb_window
{
    const char *title;

    int x;
    int y;
    int w;
    int h;

    int created;
    int open;

    int dragging;
    int drag_off_x;
    int drag_off_y;

    int header_h;
    int content_x;
    int content_y;
    int content_w;
    int content_h;

    int close_x;
    int close_y;
    int close_w;
    int close_h;

} FB_Window;

void fb_createWindow(FB_Window *win);

void fb_openWindow(FB_Window *win);

void fb_closeWindow(FB_Window *win);

void fb_drawWindow(FB_Window *win);

void fb_window_input_begin_frame(void);

void fb_window_content(FB_Window *win);

void fb_window_end_content(FB_Window *win);

void fb_window_fill_rect(FB_Window *win, int x, int y, int w, int h, uint32_t color);
void fb_window_draw_rect(FB_Window *win, int x, int y, int w, int h, uint32_t color);
void fb_window_draw_text(FB_Window *win, int x, int y, const char *text, uint32_t color);

void fb_window_draw_button(FB_Window *win, FB_Button *btn);

int fb_window_mouse_in_content(FB_Window *win);

void fb_window_focus(FB_Window *win);

#endif