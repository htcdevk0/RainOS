#include "home.h"
#include "../fb/framebuffer.h"
#include "../modules/welcome_app.h"
#include "../icons/generic_app.h"

extern int mouse_x, mouse_y;
extern int mouse_left_pressed_once;

static int home_open = 0;

static int home_x, home_y, home_w, home_h;

static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static void system_shutdown(void)
{
    __asm__ volatile("cli");

    outw(0x604, 0x2000);

    outw(0xB004, 0x2000);

    outw(0x64, 0xFE);

    for (;;)
        __asm__ volatile("hlt");
}

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

static void fill_bevel_rect(int x, int y, int w, int h, uint32_t color)
{
    if (w <= 0 || h <= 0)
        return;

    if (w < 5 || h < 5)
    {
        fb_fill_rect(x, y, w, h, color);
        return;
    }

    fb_fill_rect(x + 2, y, w - 4, h, color);
    fb_fill_rect(x, y + 2, w, h - 4, color);
}

static void draw_bevel_rect_outline(int x, int y, int w, int h,
                                    uint32_t light, uint32_t dark)
{
    if (w <= 0 || h <= 0)
        return;

    if (w < 5 || h < 5)
    {
        fb_draw_rect(x, y, w, h, dark);
        return;
    }

    fb_draw_hline(x + 2, y, w - 4, light);
    fb_put_pixel(x + 1, y + 1, light);
    fb_put_pixel(x + w - 2, y + 1, light);

    fb_draw_vline(x, y + 2, h - 4, light);
    fb_put_pixel(x + 1, y + 1, light);
    fb_put_pixel(x + 1, y + h - 2, light);

    fb_draw_hline(x + 2, y + h - 1, w - 4, dark);
    fb_put_pixel(x + 1, y + h - 2, dark);
    fb_put_pixel(x + w - 2, y + h - 2, dark);

    fb_draw_vline(x + w - 1, y + 2, h - 4, dark);
    fb_put_pixel(x + w - 2, y + 1, dark);
    fb_put_pixel(x + w - 2, y + h - 2, dark);
}

static void draw_soft_shadow(int x, int y, int w, int h)
{
    fill_bevel_rect(x + 5, y + 5, w, h, FB_RGB(45, 45, 45));
    fill_bevel_rect(x + 2, y + 2, w, h, FB_RGB(70, 70, 70));
}

static void draw_card(int x, int y, int w, int h)
{
    fill_bevel_rect(x, y, w, h, FB_RGB(232, 236, 242));
    draw_bevel_rect_outline(x, y, w, h,
                            FB_RGB(248, 249, 251),
                            FB_RGB(150, 158, 170));
    draw_bevel_rect_outline(x + 1, y + 1, w - 2, h - 2,
                            FB_RGB(255, 255, 255),
                            FB_RGB(185, 192, 202));
}

static void draw_home_logo(int x, int y, int w, int h, int active)
{
    uint32_t blue_top = active ? FB_RGB(70, 150, 255) : FB_RGB(60, 135, 235);
    uint32_t blue_bottom = active ? FB_RGB(30, 100, 220) : FB_RGB(25, 88, 195);
    uint32_t border_dark = FB_RGB(18, 55, 135);
    uint32_t border_light = FB_RGB(150, 210, 255);
    uint32_t white = FB_RGB(255, 255, 255);

    draw_soft_shadow(x - 1, y - 1, w, h);

    for (int iy = 0; iy < h; iy++)
    {
        uint32_t rr = ((blue_top >> 16) & 0xFF) +
                      ((((int)((blue_bottom >> 16) & 0xFF) - (int)((blue_top >> 16) & 0xFF)) * iy) / (h ? h : 1));
        uint32_t gg = ((blue_top >> 8) & 0xFF) +
                      ((((int)((blue_bottom >> 8) & 0xFF) - (int)((blue_top >> 8) & 0xFF)) * iy) / (h ? h : 1));
        uint32_t bb = ((blue_top) & 0xFF) +
                      ((((int)((blue_bottom) & 0xFF) - (int)((blue_top) & 0xFF)) * iy) / (h ? h : 1));

        fb_draw_hline(x + 2, y + iy, w - 4, FB_RGB(rr, gg, bb));
        if (iy >= 2 && iy < h - 2)
            fb_draw_hline(x, y + iy, w, FB_RGB(rr, gg, bb));
    }

    draw_bevel_rect_outline(x, y, w, h, border_light, border_dark);
    fb_draw_hline(x + 5, y + 3, w - 10, FB_RGB(190, 225, 255));

    int bx = x + 10;
    int by = y + 7;

    for (int i = 0; i < 16; i++)
    {
        int skew = i / 3;
        fb_draw_hline(bx + skew, by + i, 4, white);
    }

    for (int i = 0; i < 8; i++)
    {
        int skew = i / 3;
        int width = 10 - i / 2;
        if (width > 0)
            fb_draw_hline(bx + 4 + skew, by + i, width, white);
    }

    for (int i = 2; i < 7; i++)
    {
        int skew = i / 3;
        int width = 4 - i / 2;
        if (width > 0)
            fb_draw_hline(bx + 7 + skew, by + i, width, FB_RGB(70, 140, 245));
    }

    for (int i = 0; i < 11; i++)
    {
        int px = bx + 7 + i / 2 + i / 3;
        int py = by + 8 + i;
        fb_draw_hline(px, py, 4, white);
    }
}

static void draw_shutdown_button(int x, int y, int w, int h)
{
    int hover = point_in_rect(mouse_x, mouse_y, x, y, w, h);

    uint32_t top = hover ? FB_RGB(245, 105, 105) : FB_RGB(220, 85, 85);
    uint32_t bottom = hover ? FB_RGB(195, 60, 60) : FB_RGB(175, 48, 48);
    uint32_t border_dark = FB_RGB(90, 20, 20);
    uint32_t border_light = hover ? FB_RGB(255, 165, 165) : FB_RGB(235, 125, 125);
    uint32_t white = FB_RGB(255, 255, 255);

    for (int iy = 0; iy < h; iy++)
    {
        uint32_t rr = ((top >> 16) & 0xFF) +
                      ((((int)((bottom >> 16) & 0xFF) - (int)((top >> 16) & 0xFF)) * iy) / (h ? h : 1));
        uint32_t gg = ((top >> 8) & 0xFF) +
                      ((((int)((bottom >> 8) & 0xFF) - (int)((top >> 8) & 0xFF)) * iy) / (h ? h : 1));
        uint32_t bb = ((top) & 0xFF) +
                      ((((int)((bottom) & 0xFF) - (int)((top) & 0xFF)) * iy) / (h ? h : 1));

        fb_draw_hline(x + 2, y + iy, w - 4, FB_RGB(rr, gg, bb));
        if (iy >= 2 && iy < h - 2)
            fb_draw_hline(x, y + iy, w, FB_RGB(rr, gg, bb));
    }

    draw_bevel_rect_outline(x, y, w, h, border_light, border_dark);
    fb_draw_hline(x + 5, y + 3, w - 10, border_light);

    int cx = x + w / 2;
    int cy = y + h / 2 + 1;

    fb_draw_hline(cx - 6, cy + 5, 13, white);
    fb_draw_hline(cx - 7, cy + 4, 2, white);
    fb_draw_hline(cx + 5, cy + 4, 2, white);

    fb_draw_hline(cx - 8, cy + 3, 2, white);
    fb_draw_hline(cx + 6, cy + 3, 2, white);

    fb_draw_hline(cx - 8, cy + 2, 1, white);
    fb_draw_hline(cx + 7, cy + 2, 1, white);

    fb_draw_vline(cx - 8, cy - 1, 3, white);
    fb_draw_vline(cx + 8, cy - 1, 3, white);

    fb_draw_hline(cx - 7, cy - 2, 2, white);
    fb_draw_hline(cx + 5, cy - 2, 2, white);

    fb_draw_vline(cx, cy - 8, 8, white);

    if (hover && mouse_left_pressed_once)
        system_shutdown();
}

static void draw_frame(int btn_x, int btn_y)
{
    home_w = 390;
    home_h = 285;

    home_x = btn_x - 2;
    home_y = btn_y - home_h - 22;

    if (home_x + home_w > (int)g_framebuffer.width)
        home_x = (int)g_framebuffer.width - home_w - 10;
    if (home_x < 10)
        home_x = 10;
    if (home_y < 10)
        home_y = 10;

    draw_soft_shadow(home_x, home_y, home_w, home_h);

    fill_bevel_rect(home_x, home_y, home_w, home_h, FB_RGB(198, 203, 211));
    draw_bevel_rect_outline(home_x, home_y, home_w, home_h,
                            FB_RGB(232, 235, 240),
                            FB_RGB(84, 90, 100));

    int header_h = 48;
    fill_bevel_rect(home_x + 2, home_y + 2, home_w - 4, header_h, FB_RGB(182, 188, 198));
    draw_bevel_rect_outline(home_x + 2, home_y + 2, home_w - 4, header_h,
                            FB_RGB(220, 225, 232),
                            FB_RGB(120, 128, 140));

    fb_draw_hline(home_x + 14, home_y + header_h + 2, home_w - 28, FB_RGB(145, 152, 163));

    fb_draw_text(home_x + 21, home_y + 18, "RainOS", FB_RGB(90, 95, 100));
    fb_draw_text(home_x + 20, home_y + 17, "RainOS", FB_RGB(35, 38, 44));

    fb_draw_text(home_x + 20, home_y + 32, "Home", FB_RGB(88, 92, 98));
}

void home_draw(void)
{
    int taskbar_h = 50;
    int taskbar_y = (int)g_framebuffer.height - taskbar_h;

    int home_btn_x = 14;
    int home_btn_y = taskbar_y + (taskbar_h - 34) / 2;
    int home_btn_w = 40;
    int home_btn_h = 34;

    draw_home_logo(home_btn_x, home_btn_y, home_btn_w, home_btn_h, home_open);

    if (mouse_left_pressed_once &&
        point_in_rect(mouse_x, mouse_y,
                      home_btn_x, home_btn_y,
                      home_btn_w, home_btn_h))
    {
        home_open = !home_open;
    }

    if (!home_open)
        return;

    draw_frame(home_btn_x, home_btn_y);

    int card_x = home_x + 20;
    int card_y = home_y + 64;
    int card_w = 150;
    int card_h = 150;

    draw_card(card_x, card_y, card_w, card_h);

    fb_draw_text(card_x + 16, card_y + 14, "Apps", FB_RGB(55, 60, 66));

    FB_Button welcome_btn = {
        .x = card_x + 16,
        .y = card_y + 38,
        .w = 56,
        .h = 56,
        .text = NULL,
        .color = FB_RGB(220, 220, 220),
        .on_click = 0,
        .icon = &icon_generic_app};

    fb_draw_button(&welcome_btn);

    fb_draw_text(card_x + 16, card_y + 108, "Welcome", FB_RGB(35, 40, 45));
    fb_draw_text(card_x + 16, card_y + 124, "Open demo app", FB_RGB(95, 100, 108));

    if (mouse_left_pressed_once &&
        point_in_rect(mouse_x, mouse_y,
                      welcome_btn.x,
                      welcome_btn.y,
                      welcome_btn.w,
                      welcome_btn.h))
    {
        welcome_app_open();
        home_open = 0;
    }

    int side_x = home_x + 190;
    int side_y = home_y + 64;
    int side_w = 180;
    int side_h = 150;

    draw_card(side_x, side_y, side_w, side_h);

    fb_draw_text(side_x + 16, side_y + 14, "System", FB_RGB(55, 60, 66));
    fb_draw_text(side_x + 16, side_y + 42, "Minimal desktop", FB_RGB(35, 40, 45));
    fb_draw_text(side_x + 16, side_y + 58, "Kernel-space UI", FB_RGB(35, 40, 45));
    fb_draw_text(side_x + 16, side_y + 74, "RainOS shell", FB_RGB(35, 40, 45));
    fb_draw_text(side_x + 16, side_y + 106, "Fast. Weird. Fun.", FB_RGB(95, 100, 108));

    int shut_w = 42;
    int shut_h = 42;
    int shut_x = home_x + home_w - shut_w - 20;
    int shut_y = home_y + home_h - shut_h - 18;

    draw_shutdown_button(shut_x, shut_y, shut_w, shut_h);

    fb_draw_text(home_x + 20, home_y + home_h - 26, "Power", FB_RGB(85, 90, 98));
}