#ifndef RAINOS_MULTIBOOT2_H
#define RAINOS_MULTIBOOT2_H

#include "types.h"
#include <stdint-gcc.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36D76289

#define MULTIBOOT_TAG_TYPE_END          0
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER  8

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed)) multiboot_info_t;

typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) multiboot_tag_t;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
} __attribute__((packed)) multiboot_tag_framebuffer_t;

#endif