#ifndef RTC_H
#define RTC_H

typedef struct {
    int second;
    int minute;
    int hour;
} RTC_Time;

void rtc_read(RTC_Time* t);

#endif