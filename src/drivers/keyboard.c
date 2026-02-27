#include "keyboard.h"
#include "../kernel/io.h"
#include "../kernel/pic.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

volatile char keyboard_last_char = 0;

static const char scancode_table[128] = {
    0,
    27,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n',
    0,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0,
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0,
    '*',
    0,
    ' ',
    0,
};

static int keyboard_has_data(void)
{
    return inb(KEYBOARD_STATUS_PORT) & 1;
}

void keyboard_init(void)
{

    pic_clear_mask(1);
}

void keyboard_handle_interrupt(void)
{
    if (!keyboard_has_data())
        return;

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80)
        return;

    if (scancode < 128)
    {
        char c = scancode_table[scancode];
        keyboard_last_char = c;
    }
}