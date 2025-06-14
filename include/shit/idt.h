#ifndef IDT_H
#define IDT_H

#include <stdint.h>

typedef struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

typedef struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr_t;

#define IDT_MAX_DESCRIPTORS 256

#define IDT_ATTR_PRESENT 0x80
#define IDT_ATTR_RING0   0x00
#define IDT_ATTR_RING3   0x60

#define IDT_TYPE_INTERRUPT_GATE 0x0E
#define IDT_TYPE_TRAP_GATE      0x0F

/**
 * @brief Initializes the Interrupt Descriptor Table (IDT).
 */
void idt_init(void);

void idt_set_gate(uint8_t vector_num, uint64_t isr_address,
                  uint16_t cs_selector, uint8_t attributes, uint8_t ist);

// Exception ISRs
extern void isr0(void);  // Divide by Zero Error
extern void isr1(void);  // Debug
extern void isr2(void);  // Non-Maskable Interrupt
extern void isr3(void);  // Breakpoint
extern void isr4(void);  // Overflow
extern void isr5(void);  // Bound Range Exceeded
extern void isr6(void);  // Invalid Opcode
extern void isr7(void);  // Device Not Available
extern void isr8(void);  // Double Fault
extern void isr9(void);  // Coprocessor Segment Overrun (legacy)
extern void isr10(void); // Invalid TSS
extern void isr11(void); // Segment Not Present
extern void isr12(void); // Stack-Segment Fault
extern void isr13(void); // General Protection Fault
extern void isr14(void); // Page Fault
// ISR 15 is reserved
extern void isr16(void); // x87 Floating-Point Exception
extern void isr17(void); // Alignment Check
extern void isr18(void); // Machine Check
extern void isr19(void); // SIMD Floating-Point Exception
extern void isr20(void); // Virtualization Exception

// Hardware IRQ Handler Stubs
extern void irq_stub_0(void);
extern void irq_stub_1(void);
extern void irq_stub_2(void);
extern void irq_stub_3(void);
extern void irq_stub_4(void);
extern void irq_stub_5(void);
extern void irq_stub_6(void);
extern void irq_stub_7(void);
extern void irq_stub_8(void);
extern void irq_stub_9(void);
extern void irq_stub_10(void);
extern void irq_stub_11(void);
extern void irq_stub_12(void);
extern void irq_stub_13(void);
extern void irq_stub_14(void);
extern void irq_stub_15(void);

#endif // IDT_H