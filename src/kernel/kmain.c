#include "types.h"
#include "multiboot2.h"
#include "idt.h"
#include "pic.h"
#include "../fb/framebuffer.h"
#include "../drivers/mouse.h"
#include "../drivers/keyboard.h"
#include "../drivers/rtc.h"
#include "../fb/font.h"
#include <stdint-gcc.h>

#include "../desktop/home.h"

#include "../modules/window.h"
#include "../modules/welcome_app.h"

#include "../icons/generic_app.h"

static int last_second = -1;

static void cpu_hang(void)
{
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

static multiboot_tag_framebuffer_t *find_framebuffer_tag(multiboot_info_t *mbi)
{
    uint8_t *ptr = (uint8_t *)mbi + 8;
    uint8_t *end = (uint8_t *)mbi + mbi->total_size;

    while (ptr < end)
    {
        multiboot_tag_t *tag = (multiboot_tag_t *)ptr;

        if (tag->type == MULTIBOOT_TAG_TYPE_END)
            break;

        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            return (multiboot_tag_framebuffer_t *)tag;
        }

        ptr += (tag->size + 7) & ~7;
    }

    return 0;
}

extern int mouse_x, mouse_y;
extern volatile int mouse_left_down;
extern volatile int mouse_dirty;

static int prev_left_down = 0;
int mouse_left_pressed_once = 0;

void ui_input_update(void)
{
    int now = (mouse_left_down != 0);
    mouse_left_pressed_once = (now && !prev_left_down);
    prev_left_down = now;
}

int point_in_rect(int px, int py, int x, int y, int w, int h)
{
    return (px >= x && px < x + w && py >= y && py < y + h);
}

static void draw_background(void)
{

    fb_draw_gradient_vertical(FB_RGB(140, 185, 235), FB_RGB(215, 232, 250));
}

static void draw_bar(int y, int height)
{

    uint32_t matte = FB_RGB(70, 72, 76);
    fb_fill_rect(0, y, (int)g_framebuffer.width, height, matte);
}

static void draw_separator(int y)
{
    uint32_t line = FB_RGB(105, 108, 112);
    fb_draw_hline(0, y, (int)g_framebuffer.width, line);
}

static void draw_cursor(int x, int y)
{
    uint32_t black = FB_RGB(0, 0, 0);
    uint32_t white = FB_RGB(255, 255, 255);
    uint32_t shadow = FB_RGB(0, 0, 0);
    uint32_t aa_gray = FB_RGB(192, 192, 192);

    const int H = 24;
    const int OUTER_MAX = 14;
    const int INNER_MAX = 12;

    for (int i = 0; i < H; i++)
    {
        int w = (i * (OUTER_MAX + 3)) / (H - 1);
        for (int j = 0; j <= w; j++)
        {
            fb_put_pixel(x + j + 2, y + i + 2, shadow);
        }
    }
    for (int i = 0; i < H; i++)
    {
        int w = (i * (OUTER_MAX + 2)) / (H - 1) + 1;
        for (int j = 0; j <= w; j++)
        {
            fb_put_pixel(x + j + 1, y + i + 1, shadow);
        }
    }

    for (int i = 0; i < H; i++)
    {
        int w = (i * OUTER_MAX) / (H - 1);
        for (int j = 0; j <= w; j++)
        {
            fb_put_pixel(x + j, y + i, white);
        }
    }

    for (int i = 1; i < H - 1; i++)
    {
        int w = (i * INNER_MAX) / (H - 1);
        if (w > 0)
        {
            for (int j = 1; j <= w; j++)
            {
                fb_put_pixel(x + j, y + i, black);
            }
        }
    }

    for (int i = 0; i < H; i++)
    {
        int w = (i * OUTER_MAX) / (H - 1);
        fb_put_pixel(x + w + 1, y + i, aa_gray);
    }

    fb_put_pixel(x, y, white);
    fb_put_pixel(x + 1, y + 1, black);
    fb_put_pixel(x + 1, y + 2, black);

    int base_w = OUTER_MAX;
    for (int j = 0; j <= base_w; j++)
    {
        fb_put_pixel(x + j, y + H - 1, white);
    }
}

static void ui_redraw(void)
{
    fb_window_input_begin_frame();
    draw_background();

    int infobar_h = 40;
    draw_bar(0, infobar_h);
    draw_separator(infobar_h);

    int title_x = 20;
    int title_y = (infobar_h - FONT_HEIGHT) / 2;

    RTC_Time now;
    rtc_read(&now);

    char time_str[9];

    time_str[0] = '0' + (now.hour / 10);
    time_str[1] = '0' + (now.hour % 10);
    time_str[2] = ':';
    time_str[3] = '0' + (now.minute / 10);
    time_str[4] = '0' + (now.minute % 10);
    time_str[5] = ':';
    time_str[6] = '0' + (now.second / 10);
    time_str[7] = '0' + (now.second % 10);
    time_str[8] = 0;

    int tx = g_framebuffer.width - 90;
    int ty = title_y;

    fb_draw_text(tx, ty, time_str, FB_RGB(235, 235, 235));

    fb_draw_text(title_x + 1, title_y + 1, "RainOS", FB_RGB(0, 0, 0));
    fb_draw_text(title_x, title_y, "RainOS", FB_RGB(235, 235, 235));

    int taskbar_h = 50;
    int taskbar_y = (int)g_framebuffer.height - taskbar_h;
    draw_bar(taskbar_y, taskbar_h);
    draw_separator(taskbar_y - 1);

    home_draw();

    welcome_app_draw_desktop();
    welcome_app_draw_windows();

    draw_cursor(mouse_x, mouse_y);
    fb_swap_buffers();
}

void irq_handler_c(uint32_t int_no)
{
    if (int_no >= 0x20 && int_no <= 0x2F)
    {
        uint8_t irq = (uint8_t)(int_no - 0x20);

        if (irq == 1)
        {
            keyboard_handle_interrupt();
        }

        if (irq == 12)
        {
            mouse_handle_interrupt();
        }

        pic_send_eoi(irq);
    }
}

void exception_handler_c(uint32_t int_no)
{
    (void)int_no;
    for (;;)
    {
        __asm__ volatile("cli; hlt");
    }
}

void kmain(uint32_t multiboot_magic, uint32_t multiboot_addr)
{
    if (multiboot_magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    {
        cpu_hang();
    }

    multiboot_info_t *mbi = (multiboot_info_t *)(uintptr_t)multiboot_addr;
    multiboot_tag_framebuffer_t *fbtag = find_framebuffer_tag(mbi);
    if (!fbtag)
        cpu_hang();
    uint64_t fb_addr = fbtag->framebuffer_addr;

    fb_init(fb_addr,
            fbtag->framebuffer_width,
            fbtag->framebuffer_height,
            fbtag->framebuffer_pitch,
            fbtag->framebuffer_bpp);

    generic_app_icon_init();

    idt_init();
    pic_remap(0x20, 0x28);

    for (uint8_t i = 0; i < 16; i++)
    {
        pic_set_mask(i);
    }

    pic_clear_mask(2);
    pic_clear_mask(12);
    pic_clear_mask(1);

    mouse_init();
    __asm__ volatile("sti");
    welcome_app_init();

    ui_redraw();
    mouse_dirty = 0;

    for (;;)
    {

        RTC_Time now;
        rtc_read(&now);

        int need_redraw = 0;

        if (now.second != last_second)
        {
            last_second = now.second;
            need_redraw = 1;
        }

        if (mouse_dirty || need_redraw)
        {
            ui_input_update();
            ui_redraw();
            welcome_app_tick();
            mouse_dirty = 0;
        }

        __asm__ volatile("hlt");
    }
}