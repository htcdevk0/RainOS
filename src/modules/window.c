#include "window.h"
#include "font.h"
#include <stdint-gcc.h>

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_dirty;

static int g_prev_left_down = 0;
static int g_mouse_left_pressed_once = 0;
static int g_mouse_left_released_once = 0;

static FB_Window *g_content_win = 0;
static int g_content_active = 0;

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < (x + w) && py >= y && py < (y + h));
}

static int clampi(int v, int minv, int maxv)
{
    if (v < minv)
        return minv;
    if (v > maxv)
        return maxv;
    return v;
}

int fb_strlen_local(const char *s)
{
    int n = 0;
    while (s && s[n])
        n++;
    return n;
}

static void draw_hline_safe(int x, int y, int w, uint32_t c)
{
    if (w <= 0)
        return;
    fb_draw_hline(x, y, w, c);
}

static void draw_shadow_rect(int x, int y, int w, int h)
{

    fb_fill_rect(x + 4, y + 4, w, h, FB_RGB(0, 0, 0));
    fb_fill_rect(x + 2, y + 2, w, h, FB_RGB(20, 20, 20));
}

void fb_window_input_begin_frame(void)
{
    int now = (mouse_left_down != 0);
    g_mouse_left_pressed_once = (now && !g_prev_left_down);
    g_mouse_left_released_once = (!now && g_prev_left_down);
    g_prev_left_down = now;
}

void fb_createWindow(FB_Window *win)
{
    if (!win)
        return;

    if (!win->created)
    {

        win->header_h = 28;

        if (win->x == 0 && win->y == 0)
        {
            win->x = ((int)g_framebuffer.width - win->w) / 2;
            win->y = ((int)g_framebuffer.height - win->h) / 2 - 20;
            if (win->x < 8)
                win->x = 8;
            if (win->y < 8)
                win->y = 8;
        }

        win->dragging = 0;
        win->drag_off_x = 0;
        win->drag_off_y = 0;

        win->close_w = 18;
        win->close_h = 18;

        win->created = 1;
    }

    win->open = 1;
    mouse_dirty = 1;
}

void fb_openWindow(FB_Window *win)
{
    if (!win)
        return;

    if (!win->created)
    {
        fb_createWindow(win);
        return;
    }

    win->open = 1;
    mouse_dirty = 1;
}

void fb_closeWindow(FB_Window *win)
{
    if (!win)
        return;

    win->open = 0;
    win->dragging = 0;
    mouse_dirty = 1;
}

void fb_window_focus(FB_Window *win)
{
    (void)win;
}

static void fb_window_update_layout(FB_Window *win)
{
    if (!win)
        return;

    if (win->w < 140)
        win->w = 140;
    if (win->h < 90)
        win->h = 90;

    win->x = clampi(win->x, 0, (int)g_framebuffer.width - win->w);
    win->y = clampi(win->y, 0, (int)g_framebuffer.height - win->h);

    if (win->header_h <= 0)
        win->header_h = 28;

    win->content_x = win->x + 1;
    win->content_y = win->y + win->header_h + 1;
    win->content_w = win->w - 2;
    win->content_h = win->h - win->header_h - 2;

    if (win->content_w < 1)
        win->content_w = 1;
    if (win->content_h < 1)
        win->content_h = 1;

    win->close_w = 18;
    win->close_h = 18;
    win->close_x = win->x + win->w - win->close_w - 6;
    win->close_y = win->y + (win->header_h - win->close_h) / 2;
}

static void fb_window_handle_drag_and_close(FB_Window *win)
{
    if (!win || !win->open)
        return;

    int on_close = point_in_rect(mouse_x, mouse_y, win->close_x, win->close_y, win->close_w, win->close_h);

    int on_header = point_in_rect(mouse_x, mouse_y, win->x, win->y, win->w, win->header_h);
    if (on_close)
        on_header = 0;

    if (g_mouse_left_pressed_once && on_close)
    {
        fb_closeWindow(win);
        return;
    }

    if (g_mouse_left_pressed_once && on_header)
    {
        win->dragging = 1;
        win->drag_off_x = mouse_x - win->x;
        win->drag_off_y = mouse_y - win->y;
        fb_window_focus(win);
    }

    if (g_mouse_left_released_once)
    {
        win->dragging = 0;
    }

    if (win->dragging && mouse_left_down)
    {
        win->x = mouse_x - win->drag_off_x;
        win->y = mouse_y - win->drag_off_y;

        win->x = clampi(win->x, 0, (int)g_framebuffer.width - win->w);
        win->y = clampi(win->y, 0, (int)g_framebuffer.height - win->h);

        mouse_dirty = 1;
    }
}

static void fb_window_draw_chrome(FB_Window *win)
{

    uint32_t border_dark = FB_RGB(55, 58, 62);
    uint32_t border_mid = FB_RGB(108, 112, 118);
    uint32_t body_bg = FB_RGB(241, 243, 246);
    uint32_t body_inner = FB_RGB(252, 252, 252);

    uint32_t header_top = FB_RGB(96, 101, 108);
    uint32_t header_bottom = FB_RGB(70, 73, 78);
    uint32_t header_hi = FB_RGB(140, 145, 151);

    uint32_t title_color = FB_RGB(245, 245, 245);
    uint32_t title_shadow = FB_RGB(0, 0, 0);

    draw_shadow_rect(win->x, win->y, win->w, win->h);

    fb_fill_rect(win->x, win->y, win->w, win->h, border_dark);

    fb_fill_rect(win->x + 1, win->y + 1, win->w - 2, win->h - 2, border_mid);

    int hx = win->x + 2;
    int hy = win->y + 2;
    int hw = win->w - 4;
    int hh = win->header_h - 2;
    if (hh < 1)
        hh = 1;

    for (int i = 0; i < hh; i++)
    {
        uint32_t r = ((header_top >> 16) & 0xFF) + (((int)((header_bottom >> 16) & 0xFF) - (int)((header_top >> 16) & 0xFF)) * i) / (hh ? hh : 1);
        uint32_t g = ((header_top >> 8) & 0xFF) + (((int)((header_bottom >> 8) & 0xFF) - (int)((header_top >> 8) & 0xFF)) * i) / (hh ? hh : 1);
        uint32_t b = ((header_top) & 0xFF) + (((int)((header_bottom) & 0xFF) - (int)((header_top) & 0xFF)) * i) / (hh ? hh : 1);
        fb_draw_hline(hx, hy + i, hw, FB_RGB(r, g, b));
    }

    draw_hline_safe(win->x + 2, win->y + 2, win->w - 4, header_hi);

    fb_fill_rect(win->content_x, win->content_y, win->content_w, win->content_h, body_bg);
    fb_fill_rect(win->content_x + 1, win->content_y + 1, win->content_w - 2, win->content_h - 2, body_inner);

    fb_draw_hline(win->x + 1, win->y + win->header_h, win->w - 2, FB_RGB(45, 48, 52));

    int tx = win->x + 10;
    int ty = win->y + (win->header_h - FONT_HEIGHT) / 2;
    fb_draw_text(tx + 1, ty + 1, win->title ? win->title : "Window", title_shadow);
    fb_draw_text(tx, ty, win->title ? win->title : "Window", title_color);

    {
        int bx = win->close_x;
        int by = win->close_y;
        int bw = win->close_w;
        int bh = win->close_h;

        int hover = point_in_rect(mouse_x, mouse_y, bx, by, bw, bh);

        uint32_t cb = hover ? FB_RGB(225, 96, 96) : FB_RGB(198, 78, 78);
        uint32_t cb2 = hover ? FB_RGB(240, 130, 130) : FB_RGB(224, 102, 102);

        fb_fill_rect(bx, by, bw, bh, cb);
        fb_draw_rect(bx, by, bw, bh, FB_RGB(45, 20, 20));
        fb_draw_rect(bx + 1, by + 1, bw - 2, bh - 2, cb2);

        int ix0 = bx + 5;
        int iy0 = by + 5;
        int ix1 = bx + bw - 6;
        int iy1 = by + bh - 6;
        fb_draw_line(ix0, iy0, ix1, iy1, FB_RGB(255, 255, 255));
        fb_draw_line(ix1, iy0, ix0, iy1, FB_RGB(255, 255, 255));
    }
}

void fb_drawWindow(FB_Window *win)
{
    if (!win)
        return;
    if (!win->created)
        return;
    if (!win->open)
        return;

    fb_window_update_layout(win);
    fb_window_handle_drag_and_close(win);

    if (!win->open)
        return;

    fb_window_update_layout(win);
    fb_window_draw_chrome(win);
}

void fb_window_content(FB_Window *win)
{
    if (!win)
        return;
    if (!win->created || !win->open)
        return;

    g_content_win = win;
    g_content_active = 1;
}

void fb_window_end_content(FB_Window *win)
{
    (void)win;
    g_content_active = 0;
    g_content_win = 0;
}

static void to_abs(FB_Window *win, int *x, int *y)
{
    if (!win || !x || !y)
        return;
    *x = win->content_x + *x;
    *y = win->content_y + *y;
}

void fb_window_fill_rect(FB_Window *win, int x, int y, int w, int h, uint32_t color)
{
    if (!win || !win->open)
        return;
    to_abs(win, &x, &y);
    fb_fill_rect(x, y, w, h, color);
}

void fb_window_draw_rect(FB_Window *win, int x, int y, int w, int h, uint32_t color)
{
    if (!win || !win->open)
        return;
    to_abs(win, &x, &y);
    fb_draw_rect(x, y, w, h, color);
}

void fb_window_draw_text(FB_Window *win, int x, int y, const char *text, uint32_t color)
{
    if (!win || !win->open)
        return;
    to_abs(win, &x, &y);
    fb_draw_text(x, y, text, color);
}

void fb_window_draw_button(FB_Window *win, FB_Button *btn)
{
    if (!win || !win->open || !btn)
        return;

    FB_Button tmp = *btn;
    tmp.x = win->content_x + btn->x;
    tmp.y = win->content_y + btn->y;
    tmp.icon = NULL;

    fb_draw_button(&tmp);
}

int fb_window_mouse_in_content(FB_Window *win)
{
    if (!win || !win->open)
        return 0;
    return point_in_rect(mouse_x, mouse_y, win->content_x, win->content_y, win->content_w, win->content_h);
}

void fb_window_draw_textarea(FB_Window *win, FB_TextArea *ta)
{
    if (!win || !win->open || !ta)
        return;

    FB_TextArea tmp = *ta;
    tmp.x = win->content_x + ta->x;
    tmp.y = win->content_y + ta->y;

    fb_draw_textarea(&tmp);

    ta->focused = tmp.focused;
}

void fb_window_textarea_handle_input(FB_Window *win, FB_TextArea *ta)
{
    (void)win;

    if (!ta)
        return;

    fb_textarea_handle_input(ta);
}