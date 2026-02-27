#include "mouse.h"
#include "../kernel/io.h"
#include "../fb/framebuffer.h"

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_COMMAND_PORT 0x64

int mouse_x = 0;
int mouse_y = 0;
volatile int mouse_left_down = 0;

static uint8_t mouse_cycle = 0;
static int8_t mouse_packet[3];

volatile int mouse_dirty = 0;

static void mouse_wait(uint8_t type)
{
    uint32_t timeout = 100000;

    if (type == 0)
    {
        while (timeout--)
        {
            if (inb(MOUSE_STATUS_PORT) & 1)
                return;
        }
    }
    else
    {
        while (timeout--)
        {
            if ((inb(MOUSE_STATUS_PORT) & 2) == 0)
                return;
        }
    }
}

static void mouse_write(uint8_t data)
{
    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0xD4);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, data);
}

static uint8_t mouse_read(void)
{
    mouse_wait(0);
    return inb(MOUSE_DATA_PORT);
}

void mouse_init(void)
{

    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0xA8);

    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0x20);
    mouse_wait(0);

    uint8_t status = inb(MOUSE_DATA_PORT);
    status |= 0x02;
    status &= ~0x20;

    mouse_wait(1);
    outb(MOUSE_COMMAND_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();

    mouse_x = (int)g_framebuffer.width / 2;
    mouse_y = (int)g_framebuffer.height / 2;
    mouse_dirty = 1;
}

void mouse_handle_interrupt(void)
{
    uint8_t status = inb(MOUSE_STATUS_PORT);

    if (!(status & 0x20))
        return;

    int8_t data = (int8_t)inb(MOUSE_DATA_PORT);

    if (mouse_cycle == 0)
    {
        mouse_packet[0] = data;
        mouse_cycle = 1;
        return;
    }

    if (mouse_cycle == 1)
    {
        mouse_packet[1] = data;
        mouse_cycle = 2;
        return;
    }

    mouse_packet[2] = data;
    mouse_cycle = 0;

    int dx = mouse_packet[1];
    int dy = mouse_packet[2];

    mouse_x += dx;
    mouse_y -= dy;

    if (mouse_x < 0)
        mouse_x = 0;
    if (mouse_y < 0)
        mouse_y = 0;
    if (mouse_x >= (int)g_framebuffer.width)
        mouse_x = (int)g_framebuffer.width - 1;
    if (mouse_y >= (int)g_framebuffer.height)
        mouse_y = (int)g_framebuffer.height - 1;

    mouse_dirty = 1;
    mouse_left_down = mouse_packet[0] & 0x01;
}