#include "rtc.h"
#include "../kernel/types.h"

#define TIMEZONE_OFFSET -3

static inline void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static unsigned char read_register(unsigned char reg)
{
    outb(0x70, reg);
    return inb(0x71);
}

static int bcd_to_bin(unsigned char val)
{
    return (val & 0x0F) + ((val >> 4) * 10);
}

void rtc_read(RTC_Time *t)
{
    unsigned char sec_raw, min_raw, hour_raw;
    unsigned char regB;

    while (read_register(0x0A) & 0x80)
        ;

    sec_raw = read_register(0x00);
    min_raw = read_register(0x02);
    hour_raw = read_register(0x04);
    regB = read_register(0x0B);

    int sec = sec_raw;
    int min = min_raw;
    int hour = hour_raw;

    int is_binary = regB & 0x04;
    int is_24h = regB & 0x02;

    if (!is_binary)
    {
        sec = bcd_to_bin(sec);
        min = bcd_to_bin(min);
        hour = bcd_to_bin(hour & 0x7F);
    }

    if (!is_24h)
    {
        int is_pm = hour_raw & 0x80;

        if (is_pm && hour != 12)
            hour += 12;
        if (!is_pm && hour == 12)
            hour = 0;
    }

    hour += TIMEZONE_OFFSET;

    if (hour < 0)
        hour += 24;
    if (hour >= 24)
        hour -= 24;

    t->second = sec;
    t->minute = min;
    t->hour = hour;
}