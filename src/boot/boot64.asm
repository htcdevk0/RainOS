BITS 32

section .multiboot2
align 8
mb2_header_start:
    dd 0xE85250D6
    dd 0
    dd mb2_header_end - mb2_header_start
    dd -(0xE85250D6 + 0 + (mb2_header_end - mb2_header_start))

    align 8
    dw 5
    dw 0
    dd 20
    dd 1920
    dd 1080
    dd 32

    align 8
    dw 0
    dw 0
    dd 8
mb2_header_end:

section .text
global _start
extern kmain

_start:
    cli
    mov esp, stack_top

    mov [saved_magic], eax
    mov [saved_mbi], ebx

    call enter_long_mode
    jmp $

enter_long_mode:
    lgdt [gdt_desc]

    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax

    lea eax, [pml4_table]
    mov cr3, eax
    mov dword [tmp_cr3], eax
    mov eax, [tmp_cr3]
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, (1 << 8)
    wrmsr

    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

    jmp 0x08:long_mode_entry

BITS 64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack64_top

    mov edi, [saved_magic]
    mov esi, [saved_mbi]

    call kmain

    hlt
    jmp $

BITS 32

section .data
align 16
tmp_cr3: dd 0

gdt64:
    dq 0
    dq 0x00AF9A000000FFFF
    dq 0x00AF92000000FFFF
gdt_end:

gdt_desc:
    dw gdt_end - gdt64 - 1
    dd gdt64

align 4096
pml4_table:
    dq pdpt_table + 0x003
    times 511 dq 0

align 4096
pdpt_table:
    dq pd_table0  + 0x003
    dq pd_table1  + 0x003
    dq pd_table2  + 0x003
    dq pd_table3  + 0x003
    dq pd_table4  + 0x003
    dq pd_table5  + 0x003
    dq pd_table6  + 0x003
    dq pd_table7  + 0x003
    dq pd_table8  + 0x003
    dq pd_table9  + 0x003
    dq pd_table10 + 0x003
    dq pd_table11 + 0x003
    dq pd_table12 + 0x003
    dq pd_table13 + 0x003
    dq pd_table14 + 0x003
    dq pd_table15 + 0x003
    times (512-16) dq 0

%macro MAKE_PD 2
align 4096
%1:
%assign i 0
%rep 512
    dq ((%2 + i) * 0x200000) + 0x083
%assign i i+1
%endrep
%endmacro

MAKE_PD pd_table0,  0
MAKE_PD pd_table1,  512
MAKE_PD pd_table2,  1024
MAKE_PD pd_table3,  1536
MAKE_PD pd_table4,  2048
MAKE_PD pd_table5,  2560
MAKE_PD pd_table6,  3072
MAKE_PD pd_table7,  3584
MAKE_PD pd_table8,  4096
MAKE_PD pd_table9,  4608
MAKE_PD pd_table10, 5120
MAKE_PD pd_table11, 5632
MAKE_PD pd_table12, 6144
MAKE_PD pd_table13, 6656
MAKE_PD pd_table14, 7168
MAKE_PD pd_table15, 7680

saved_magic: dd 0
saved_mbi:   dd 0

section .bss
align 16
stack_bottom: resb 16384
stack_top:

align 16
stack64_bottom: resb 16384
stack64_top: