BITS 64

extern irq_handler_c
extern exception_handler_c

isr_common_stub:
    ; salva registradores gerais
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; stack layout agora:
    ; topo -> r15
    ; ...
    ; rax
    ; int_no
    ; error_code (ou 0)

    mov rdi, [rsp + 15*8]   ; pega int_no

    cmp rdi, 0x20
    jl .exception

    call irq_handler_c
    jmp .done

.exception:
    call exception_handler_c

.done:
    ; restaura registradores
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16
    iretq

%macro EXC_NOERR 1
global isr%1
isr%1:
    push qword 0
    push qword %1
    jmp isr_common_stub
%endmacro

%macro EXC_ERR 1
global isr%1
isr%1:
    push qword %1
    jmp isr_common_stub
%endmacro

%macro IRQ_STUB 2
global irq%1
irq%1:
    push qword 0
    push qword %2
    jmp isr_common_stub
%endmacro

EXC_NOERR 0
EXC_NOERR 1
EXC_NOERR 2
EXC_NOERR 3
EXC_NOERR 4
EXC_NOERR 5
EXC_NOERR 6
EXC_NOERR 7

EXC_ERR   8
EXC_NOERR 9
EXC_ERR   10
EXC_ERR   11
EXC_ERR   12
EXC_ERR   13
EXC_ERR   14

EXC_NOERR 15
EXC_NOERR 16
EXC_ERR   17
EXC_NOERR 18
EXC_NOERR 19
EXC_NOERR 20
EXC_NOERR 21
EXC_NOERR 22
EXC_NOERR 23
EXC_NOERR 24
EXC_NOERR 25
EXC_NOERR 26
EXC_NOERR 27
EXC_NOERR 28
EXC_NOERR 29
EXC_NOERR 30
EXC_NOERR 31

IRQ_STUB 0,  0x20
IRQ_STUB 1,  0x21
IRQ_STUB 2,  0x22
IRQ_STUB 3,  0x23
IRQ_STUB 4,  0x24
IRQ_STUB 5,  0x25
IRQ_STUB 6,  0x26
IRQ_STUB 7,  0x27
IRQ_STUB 8,  0x28
IRQ_STUB 9,  0x29
IRQ_STUB 10, 0x2A
IRQ_STUB 11, 0x2B
IRQ_STUB 12, 0x2C
IRQ_STUB 13, 0x2D
IRQ_STUB 14, 0x2E
IRQ_STUB 15, 0x2F