#ifndef RAINOS_IDT_H
#define RAINOS_IDT_H

#include "types.h"
#include <stdint-gcc.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr_t;

void idt_init(void);
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);

#endif