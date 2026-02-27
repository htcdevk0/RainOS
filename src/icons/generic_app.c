#include "generic_app.h"

#define C_TRANSPARENT 0x00000000
#define C_DOC 0xF8FAFC
#define C_BORDER 0xCBD5E1
#define C_FOLD 0xE2E8F0
#define C_TEXT 0x94A3B8

static uint32_t pixels[32 * 32];

void generic_app_icon_init(void)
{

    for (int i = 0; i < 32 * 32; i++)
        pixels[i] = C_TRANSPARENT;

    for (int y = 4; y < 28; y++)
        for (int x = 6; x < 26; x++)
            pixels[y * 32 + x] = C_DOC;

    for (int x = 6; x < 26; x++)
    {
        pixels[4 * 32 + x] = C_BORDER;
        pixels[27 * 32 + x] = C_BORDER;
    }

    for (int y = 4; y < 28; y++)
    {
        pixels[y * 32 + 6] = C_BORDER;
        pixels[y * 32 + 25] = C_BORDER;
    }

    for (int x = 9; x < 23; x++)
        pixels[12 * 32 + x] = C_TEXT;
    for (int x = 9; x < 21; x++)
        pixels[15 * 32 + x] = C_TEXT;
}

FB_Icon icon_generic_app = {
    .w = 32,
    .h = 32,
    .pixels = pixels};