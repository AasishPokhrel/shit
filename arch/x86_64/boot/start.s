section .bss
    align 16

stack_bottom:
    resb 16384
stack_top:

section .text
    bits 64

    global _start
    extern kmain

_start:
    ; At this very moment:
    ;   - We're running in 64-bit long mode.
    ;   - Paging is already set up by Limine.
    ;   - Limine has also kindly provided a higher-half direct map of physical memory.

    mov rsp, stack_top

    call kmain

    ; Ideally, kmain never returns. BUT if something unexpected happens
    ; and kmain does return, we absolutely DO NOT want the CPU to just
    ; wander off into the unknown.
    ; So, we'll politely ask it to take a nap.
.halt_loop:
    cli
    hlt
    jmp .halt_loop