#include "shell.h"
#include "shell_interpreter.h"

#include "../fb/framebuffer.h"
#include "../fb/font.h"
#include "../drivers/mouse.h"
#include "../icons/shell_icon.h"
#include "../modules/window.h"
#include "../icons/desktop.h"

static void copy_string(char *dst, const char *src, int max)
{
    int i = 0;

    if (!dst || !src || max <= 0)
        return;

    while (src[i] && i < max - 1)
    {
        dst[i] = src[i];
        i++;
    }

    dst[i] = 0;
}

static FB_Window g_win = {
    .title = "Shell",
    .x = 260,
    .y = 150,
    .w = 620,
    .h = 340,
    .created = 0,
    .open = 0};

static FB_DesktopIcon g_shell_icon = {
    .x = 560,
    .y = 90,
    .w = 100,
    .h = 100,
    .label = "Shell",
    .icon = &icon_shell_app,
    .window = &g_win};

static char g_shell_input[128] = {0};
static char g_shell_output[256] = "RainOS Shell ready.";
static char g_last_command[128] = "";

static FB_TextArea g_shell_textarea;

static void shell_submit(char *text)
{
    copy_string(g_last_command, text, sizeof(g_last_command));
    shell_interpret_command(text, g_shell_output, sizeof(g_shell_output));

    g_shell_textarea.length = 0;
    g_shell_input[0] = 0;
}

void shell_app_init(void)
{
    g_win.created = 0;
    g_win.open = 0;

    g_shell_textarea.x = 14;
    g_shell_textarea.y = 250;
    g_shell_textarea.w = 420;
    g_shell_textarea.h = 28;

    g_shell_textarea.buffer = g_shell_input;
    g_shell_textarea.buffer_size = sizeof(g_shell_input);
    g_shell_textarea.length = 0;
    g_shell_textarea.focused = 0;

    g_shell_textarea.bg_color = FB_RGB(255, 255, 255);
    g_shell_textarea.border_color = FB_RGB(90, 96, 104);
    g_shell_textarea.text_color = FB_RGB(20, 20, 20);

    g_shell_textarea.on_enter = shell_submit;

    fb_desktop_register_icon(&g_shell_icon);
}

void shell_app_open(void)
{
    if (!g_win.created)
        fb_createWindow(&g_win);
    else
        fb_openWindow(&g_win);
}

void shell_app_tick(void)
{
}

void shell_app_draw_desktop(void)
{
}

void shell_app_draw_windows(void)
{
    fb_drawWindow(&g_win);

    if (!g_win.open)
        return;

    fb_window_content(&g_win);

    fb_window_draw_text(&g_win, 14, 14, "RainOS Shell", FB_RGB(25, 25, 28));

    fb_window_fill_rect(&g_win, 14, 40, 580, 120, FB_RGB(230, 235, 242));
    fb_window_draw_rect(&g_win, 14, 40, 580, 120, FB_RGB(90, 96, 104));

    fb_window_draw_text(&g_win, 24, 52, "Last command:", FB_RGB(45, 45, 50));
    fb_window_draw_text(&g_win, 24, 72, g_last_command[0] ? g_last_command : "(none)", FB_RGB(20, 80, 30));

    fb_window_draw_text(&g_win, 24, 102, "Output:", FB_RGB(45, 45, 50));
    fb_window_draw_text(&g_win, 24, 122, g_shell_output, FB_RGB(20, 20, 20));

    fb_window_draw_text(&g_win, 14, 178, "Supported command:", FB_RGB(45, 45, 50));
    fb_window_draw_text(&g_win, 14, 198, "echo <text>", FB_RGB(20, 80, 30));

    fb_window_draw_text(&g_win, 14, 230, "Input:", FB_RGB(45, 45, 50));

    fb_window_textarea_handle_input(&g_win, &g_shell_textarea);
    fb_window_draw_textarea(&g_win, &g_shell_textarea);

    fb_window_draw_text(&g_win, 446, 256, "Press Enter", FB_RGB(80, 80, 85));

    fb_window_end_content(&g_win);
}