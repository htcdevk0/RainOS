#ifndef SETTINGS_APP_H
#define SETTINGS_APP_H

#include <stdint-gcc.h>

typedef enum
{
    WALLPAPER_DEFAULT = 0,
    WALLPAPER_DARK,
    WALLPAPER_FLAT
} WallpaperType;

typedef struct
{
    WallpaperType type;
} WallpaperState;

extern WallpaperState g_wallpaper;

void settings_app_init(void);
void settings_app_tick(void);
void settings_app_draw_desktop(void);
void settings_app_draw_windows(void);

#endif