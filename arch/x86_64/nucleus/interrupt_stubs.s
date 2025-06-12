%macro ISR_NO_ERR_CODE 1
global isr%1
isr%1:
    cli
    push byte 0
    push byte %1
    jmp common_isr_stub
%endmacro

%macro ISR_ERR_CODE 1
global isr%1
isr%1:
    cli
    push byte %1
    jmp common_isr_stub
%endmacro

common_isr_stub:
    push rax
    push rcx
    push rdx
    push rbx
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

    mov rdi, rsp
    extern exception_handler
    call exception_handler

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
    pop rbx
    pop rdx
    pop rcx
    pop rax

    add rsp, 16
    sti
    iretq

ISR_NO_ERR_CODE 0
ISR_NO_ERR_CODE 1
ISR_NO_ERR_CODE 2
ISR_NO_ERR_CODE 3
ISR_NO_ERR_CODE 4
ISR_NO_ERR_CODE 5
ISR_NO_ERR_CODE 6
ISR_NO_ERR_CODE 7
ISR_ERR_CODE 8
ISR_NO_ERR_CODE 9
ISR_ERR_CODE 10
ISR_ERR_CODE 11
ISR_ERR_CODE 12
ISR_ERR_CODE 13
ISR_ERR_CODE 14
; ISR 15 is reserved by Intel
ISR_NO_ERR_CODE 16
ISR_ERR_CODE 17
ISR_NO_ERR_CODE 18
ISR_NO_ERR_CODE 19
ISR_NO_ERR_CODE 20

%macro IRQ_HANDLER_STUB 2
global irq_stub_%1        ; Export label like irq_stub_0, irq_stub_1
irq_stub_%1:
    cli                   ; Disable further interrupts
    push byte 0           ; Push a dummy error code (0) for stack frame consistency
    push byte %2          ; Push the IDT vector number (e.g., 32 for IRQ0)
    jmp common_irq_dispatcher_stub ; Jump to a common stub for IRQs
%endmacro

common_irq_dispatcher_stub:
    push rax
    push rcx
    push rdx
    push rbx
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

    mov rdi, rsp
    extern irq_c_dispatcher
    call irq_c_dispatcher

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
    pop rbx
    pop rdx
    pop rcx
    pop rax
    
    add rsp, 16
    sti
    iretq

IRQ_HANDLER_STUB  0,  32  ; Timer (PIC IRQ 0)
IRQ_HANDLER_STUB  1,  33  ; Keyboard (PIC IRQ 1)
IRQ_HANDLER_STUB  2,  34  ; Cascade for Slave PIC (PIC IRQ 2)
IRQ_HANDLER_STUB  3,  35  ; COM2 / Serial Port B (PIC IRQ 3)
IRQ_HANDLER_STUB  4,  36  ; COM1 / Serial Port A (PIC IRQ 4)
IRQ_HANDLER_STUB  5,  37  ; LPT2 / Sound Card (PIC IRQ 5)
IRQ_HANDLER_STUB  6,  38  ; Floppy Disk Controller (PIC IRQ 6)
IRQ_HANDLER_STUB  7,  39  ; LPT1 / Spurious IRQ7 (PIC IRQ 7)
IRQ_HANDLER_STUB  8,  40  ; Real Time Clock (RTC) (PIC IRQ 8)
IRQ_HANDLER_STUB  9,  41  ; Free / ACPI / NIC / USB (PIC IRQ 9)
IRQ_HANDLER_STUB 10,  42  ; Free / NIC / USB (PIC IRQ 10)
IRQ_HANDLER_STUB 11,  43  ; Free / NIC / USB (PIC IRQ 11)
IRQ_HANDLER_STUB 12,  44  ; PS/2 Mouse (PIC IRQ 12)
IRQ_HANDLER_STUB 13,  45  ; FPU Coprocessor / Inter-processor (PIC IRQ 13)
IRQ_HANDLER_STUB 14,  46  ; Primary ATA Hard Disk (PIC IRQ 14)
IRQ_HANDLER_STUB 15,  47  ; Secondary ATA Hard Disk (PIC IRQ 15)

global idt_load:
idt_load:
    lidt [rdi]
    ret