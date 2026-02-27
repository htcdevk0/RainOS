#include "debug_app.h"

#include "../fb/framebuffer.h"
#include "../fb/font.h"
#include "../drivers/mouse.h"
#include "../icons/generic_app.h"
#include "window.h"

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_left_pressed_once;

/* =========================================================
   STATE
   ========================================================= */

static FB_Window g_win = {
    .title = "Debug App",
    .x = 280,
    .y = 150,
    .w = 560,
    .h = 360,
    .created = 0,
    .open = 0
};

static char g_input_buffer[128] = {0};
static char g_display_text[128] = "Type something and press Enter.";

static FB_TextArea g_textarea;

/* =========================================================
   CALLBACK
   ========================================================= */

static void debug_on_enter(char *text)
{
    int i = 0;

    while (i < 127 && text[i])
    {
        g_display_text[i] = text[i];
        i++;
    }

    g_display_text[i] = 0;

    g_textarea.length = 0;
    g_input_buffer[0] = 0;
}

/* =========================================================
   INIT
   ========================================================= */

void debug_app_init(void)
{
    g_win.created = 0;
    g_win.open = 0;

    g_textarea.x = 20;
    g_textarea.y = 220;
    g_textarea.w = 300;
    g_textarea.h = 28;

    g_textarea.buffer = g_input_buffer;
    g_textarea.buffer_size = sizeof(g_input_buffer);
    g_textarea.length = 0;
    g_textarea.focused = 0;

    g_textarea.bg_color = FB_RGB(255, 255, 255);
    g_textarea.border_color = FB_RGB(90, 96, 104);
    g_textarea.text_color = FB_RGB(20, 20, 20);

    g_textarea.on_enter = debug_on_enter;
}

/* =========================================================
   OPEN
   ========================================================= */

void debug_app_open(void)
{
    if (!g_win.created)
        fb_createWindow(&g_win);
    else
        fb_openWindow(&g_win);
}

/* =========================================================
   TICK
   ========================================================= */

void debug_app_tick(void)
{
    /* nada especial aqui por enquanto */
}

/* =========================================================
   DESKTOP (sem ícone ainda)
   ========================================================= */

void debug_app_draw_desktop(void)
{
    static int icon_x = 550;
    static int icon_y = 90;
    static int icon_w = 80;
    static int icon_h = 90;

    int hover =
        mouse_x >= icon_x &&
        mouse_x < icon_x + icon_w &&
        mouse_y >= icon_y &&
        mouse_y < icon_y + icon_h;

    if (hover)
    {
        fb_fill_rect(icon_x, icon_y,
                     icon_w, icon_h,
                     FB_RGB(255, 255, 255));

        fb_draw_rect(icon_x, icon_y,
                     icon_w, icon_h,
                     FB_RGB(120, 130, 145));
    }

    /* Ícone (usa generic por enquanto) */
    int scale = 2;

    int scaled_w = icon_generic_app.w * scale;
    int scaled_h = icon_generic_app.h * scale;

    int ix = icon_x + (icon_w - scaled_w) / 2;
    int iy = icon_y + ((icon_h - FONT_HEIGHT - 6) - scaled_h) / 2;

    fb_draw_icon_scaled(ix, iy, &icon_generic_app, scale);

    /* Texto */
    const char *label = "Debug";

    int len = 0;
    while (label[len]) len++;

    int tw = len * (FONT_WIDTH + 1) - 1;

    int tx = icon_x + (icon_w - tw) / 2;
    int ty = icon_y + icon_h - FONT_HEIGHT - 2;

    fb_draw_text(tx + 1, ty + 1, label, FB_RGB(0, 0, 0));
    fb_draw_text(tx, ty, label, FB_RGB(245, 245, 245));

    /* Clique */
    if (hover && mouse_left_pressed_once)
    {
        debug_app_open();
    }
}

/* =========================================================
   WINDOWS
   ========================================================= */

void debug_app_draw_windows(void)
{
    fb_drawWindow(&g_win);

    if (!g_win.open)
        return;

    fb_window_content(&g_win);

    /* Título interno */
    fb_window_draw_text(&g_win, 20, 16,
        "Debug Output:",
        FB_RGB(35, 38, 44));

    /* Texto grande exibido */
    fb_window_draw_text(&g_win, 20, 50,
        g_display_text,
        FB_RGB(20, 20, 20));

    /* instrução */
    fb_window_draw_text(&g_win, 20, 190,
        "Enter text below:",
        FB_RGB(60, 65, 70));

    /* INPUT */
    fb_window_textarea_handle_input(&g_win, &g_textarea);
    fb_window_draw_textarea(&g_win, &g_textarea);

    fb_window_end_content(&g_win);
}