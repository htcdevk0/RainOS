#include "keyboard.h"
#include "../kernel/io.h"
#include "../kernel/pic.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define KB_BUFFER_SIZE 128

static uint8_t key_state[128] = {0};

static volatile char kb_buffer[KB_BUFFER_SIZE];
static volatile int kb_head = 0;
static volatile int kb_tail = 0;

static int shift_pressed = 0;
static int caps_lock = 0;

static const char scancode_table[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0};

static const char scancode_table_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0};

static int keyboard_has_data(void)
{
    return inb(KEYBOARD_STATUS_PORT) & 1;
}

static char apply_caps_logic(char c)
{
    if (c >= 'a' && c <= 'z')
        return (caps_lock ^ shift_pressed) ? (c - 32) : c;

    if (c >= 'A' && c <= 'Z')
        return (caps_lock ^ shift_pressed) ? c : (c + 32);

    return c;
}

static void kb_push(char c)
{
    int next = (kb_head + 1) % KB_BUFFER_SIZE;

    if (next == kb_tail)
        return;

    kb_buffer[kb_head] = c;
    kb_head = next;
}

char keyboard_get_char(void)
{
    if (kb_head == kb_tail)
        return 0;

    char c = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    return c;
}

void keyboard_init(void)
{
    pic_clear_mask(1);
}

void keyboard_handle_interrupt(void)
{
    if (!keyboard_has_data())
    {
        pic_send_eoi(1);
        return;
    }

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    int released = scancode & 0x80;
    uint8_t code = scancode & 0x7F;

    if (code == 0x2A || code == 0x36)
    {
        shift_pressed = released ? 0 : 1;
        pic_send_eoi(1);
        return;
    }

    if (code == 0x3A && !released)
    {
        caps_lock = !caps_lock;
        pic_send_eoi(1);
        return;
    }

    if (code < 128)
    {
        if (released)
        {
            key_state[code] = 0;
        }
        else
        {
            if (!key_state[code])
            {
                key_state[code] = 1;

                char c = shift_pressed
                             ? scancode_table_shift[code]
                             : scancode_table[code];

                if (c)
                    kb_push(apply_caps_logic(c));
            }
        }
    }

    pic_send_eoi(1);
}