#include "idt.h"
#include <stdint-gcc.h>

#define IDT_ENTRIES 256

static idt_entry_t g_idt[IDT_ENTRIES];
static idt_ptr_t g_idt_ptr;

extern void idt_flush(idt_ptr_t *idt_ptr);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
    g_idt[num].offset_low = (uint16_t)(base & 0xFFFF);
    g_idt[num].selector = sel;
    g_idt[num].ist = 0;
    g_idt[num].type_attr = flags;
    g_idt[num].offset_mid = (uint16_t)((base >> 16) & 0xFFFF);
    g_idt[num].offset_high = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    g_idt[num].zero = 0;
}

void idt_init(void)
{
    g_idt_ptr.limit = (uint16_t)(sizeof(idt_entry_t) * IDT_ENTRIES - 1);
    g_idt_ptr.base = (uint64_t)&g_idt[0];

    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt_set_gate((uint8_t)i, 0, 0, 0);
    }

    uint8_t flags = 0x8E;
    uint16_t sel = 0x08;

    idt_set_gate(0x00, (uint64_t)isr0, sel, flags);
    idt_set_gate(0x01, (uint64_t)isr1, sel, flags);
    idt_set_gate(0x02, (uint64_t)isr2, sel, flags);
    idt_set_gate(0x03, (uint64_t)isr3, sel, flags);
    idt_set_gate(0x04, (uint64_t)isr4, sel, flags);
    idt_set_gate(0x05, (uint64_t)isr5, sel, flags);
    idt_set_gate(0x06, (uint64_t)isr6, sel, flags);
    idt_set_gate(0x07, (uint64_t)isr7, sel, flags);
    idt_set_gate(0x08, (uint64_t)isr8, sel, flags);
    idt_set_gate(0x09, (uint64_t)isr9, sel, flags);
    idt_set_gate(0x0A, (uint64_t)isr10, sel, flags);
    idt_set_gate(0x0B, (uint64_t)isr11, sel, flags);
    idt_set_gate(0x0C, (uint64_t)isr12, sel, flags);
    idt_set_gate(0x0D, (uint64_t)isr13, sel, flags);
    idt_set_gate(0x0E, (uint64_t)isr14, sel, flags);
    idt_set_gate(0x0F, (uint64_t)isr15, sel, flags);
    idt_set_gate(0x10, (uint64_t)isr16, sel, flags);
    idt_set_gate(0x11, (uint64_t)isr17, sel, flags);
    idt_set_gate(0x12, (uint64_t)isr18, sel, flags);
    idt_set_gate(0x13, (uint64_t)isr19, sel, flags);
    idt_set_gate(0x14, (uint64_t)isr20, sel, flags);
    idt_set_gate(0x15, (uint64_t)isr21, sel, flags);
    idt_set_gate(0x16, (uint64_t)isr22, sel, flags);
    idt_set_gate(0x17, (uint64_t)isr23, sel, flags);
    idt_set_gate(0x18, (uint64_t)isr24, sel, flags);
    idt_set_gate(0x19, (uint64_t)isr25, sel, flags);
    idt_set_gate(0x1A, (uint64_t)isr26, sel, flags);
    idt_set_gate(0x1B, (uint64_t)isr27, sel, flags);
    idt_set_gate(0x1C, (uint64_t)isr28, sel, flags);
    idt_set_gate(0x1D, (uint64_t)isr29, sel, flags);
    idt_set_gate(0x1E, (uint64_t)isr30, sel, flags);
    idt_set_gate(0x1F, (uint64_t)isr31, sel, flags);

    idt_set_gate(0x20, (uint64_t)irq0, sel, flags);
    idt_set_gate(0x21, (uint64_t)irq1, sel, flags);
    idt_set_gate(0x22, (uint64_t)irq2, sel, flags);
    idt_set_gate(0x23, (uint64_t)irq3, sel, flags);
    idt_set_gate(0x24, (uint64_t)irq4, sel, flags);
    idt_set_gate(0x25, (uint64_t)irq5, sel, flags);
    idt_set_gate(0x26, (uint64_t)irq6, sel, flags);
    idt_set_gate(0x27, (uint64_t)irq7, sel, flags);
    idt_set_gate(0x28, (uint64_t)irq8, sel, flags);
    idt_set_gate(0x29, (uint64_t)irq9, sel, flags);
    idt_set_gate(0x2A, (uint64_t)irq10, sel, flags);
    idt_set_gate(0x2B, (uint64_t)irq11, sel, flags);
    idt_set_gate(0x2C, (uint64_t)irq12, sel, flags);
    idt_set_gate(0x2D, (uint64_t)irq13, sel, flags);
    idt_set_gate(0x2E, (uint64_t)irq14, sel, flags);
    idt_set_gate(0x2F, (uint64_t)irq15, sel, flags);

    idt_flush(&g_idt_ptr);
}