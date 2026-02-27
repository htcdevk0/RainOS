#include "home.h"
#include "../fb/framebuffer.h"
#include "../modules/welcome_app.h"
#include "../icons/generic_app.h"

extern int mouse_x, mouse_y;
extern int mouse_left_pressed_once;

static int home_open = 0;

static int home_x, home_y, home_w, home_h;

static int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

void home_init(void)
{
    home_open = 0;
}

void home_toggle(void)
{
    home_open = !home_open;
}

static void draw_frame(int btn_x, int btn_y)
{
    home_w = 360;
    home_h = 260;

    home_x = btn_x;
    home_y = btn_y - home_h - 29;

    if (home_x + home_w > (int)g_framebuffer.width)
        home_x = (int)g_framebuffer.width - home_w - 8;
    if (home_x < 8)
        home_x = 8;
    if (home_y < 8)
        home_y = 8;

    fb_fill_rect(home_x + 4, home_y + 4, home_w, home_h, FB_RGB(120, 120, 120));

    fb_fill_rect(home_x, home_y, home_w, home_h, FB_RGB(185, 185, 185));

    fb_draw_rect(home_x, home_y, home_w, home_h, FB_RGB(80, 80, 80));

    fb_draw_rect(home_x + 1, home_y + 1, home_w - 2, home_h - 2, FB_RGB(210, 210, 210));
}

void home_draw(void)
{
    int taskbar_h = 50;
    int taskbar_y = (int)g_framebuffer.height - taskbar_h;

    FB_Button home_btn = {
        .x = 14,
        .y = taskbar_y + (taskbar_h - 34) / 2,
        .w = 40,
        .h = 34,
        .text = "R",
        .color = home_open ? FB_RGB(200, 200, 200) : FB_RGB(220, 220, 220),
        .on_click = 0,
        .icon = 0};

    fb_draw_button(&home_btn);

    if (mouse_left_pressed_once)
    {
        if (point_in_rect(mouse_x, mouse_y,
                          home_btn.x,
                          home_btn.y,
                          home_btn.w,
                          home_btn.h))
        {
            home_open = !home_open;
        }
    }

    if (!home_open)
        return;

    draw_frame(home_btn.x, home_btn.y);

    FB_Button welcome_btn = {
        .x = home_x + 20,
        .y = home_y + 20,
        .w = 40,
        .h = 40,
        .text = "Welcome",
        .color = FB_RGB(220, 220, 220),
        .on_click = 0,
        .icon = &icon_generic_app};

    fb_draw_button(&welcome_btn);

    if (mouse_left_pressed_once)
    {
        if (point_in_rect(mouse_x, mouse_y,
                          welcome_btn.x,
                          welcome_btn.y,
                          welcome_btn.w,
                          welcome_btn.h))
        {
            welcome_app_open();
            home_open = 0;
        }
    }
}