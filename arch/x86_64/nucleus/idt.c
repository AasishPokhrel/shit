#include <drivers/pic.h>
#include <shit/idt.h>
#include <shit/printk.h>
#include <stddef.h>

static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idt_ptr_t idtp;

#define KERNEL_CS 0x28

void idt_set_gate(uint8_t vector_num, uint64_t isr_address,
                  uint16_t cs_selector, uint8_t attributes, uint8_t ist) {
  idt_entry_t *descriptor = &idt[vector_num];

  descriptor->isr_low = (uint16_t)(isr_address & 0xFFFF);
  descriptor->kernel_cs = cs_selector;
  descriptor->ist = ist;
  descriptor->attributes = attributes;
  descriptor->isr_mid = (uint16_t)((isr_address >> 16) & 0xFFFF);
  descriptor->isr_high = (uint32_t)((isr_address >> 32) & 0xFFFFFFFF);
  descriptor->reserved = 0;
}

extern void idt_load(idt_ptr_t *idtp_param);

void idt_init(void) {
  idtp.base = (uint64_t)&idt[0];
  idtp.limit = (uint16_t)(sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1);

  printk(KERN_DEBUG "IDT: Initializing IDT at %p, limit 0x%x\n",
         (void *)idtp.base, idtp.limit);

  for (int i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
    idt_set_gate(i, 0, 0, 0, 0);
  }

  uint8_t gate_attributes =
      IDT_ATTR_PRESENT | IDT_TYPE_INTERRUPT_GATE | IDT_ATTR_RING0;
  uint8_t irq_gate_attributes =
      IDT_ATTR_PRESENT | IDT_TYPE_INTERRUPT_GATE | IDT_ATTR_RING0;

  idt_set_gate(0, (uint64_t)isr0, KERNEL_CS, gate_attributes,
               0); // Divide by Zero
  idt_set_gate(1, (uint64_t)isr1, KERNEL_CS, gate_attributes, 0); // Debug
  idt_set_gate(2, (uint64_t)isr2, KERNEL_CS, gate_attributes, 0); // NMI
  idt_set_gate(3, (uint64_t)isr3, KERNEL_CS, gate_attributes,
               0);                                                // Breakpoint
  idt_set_gate(4, (uint64_t)isr4, KERNEL_CS, gate_attributes, 0); // Overflow
  idt_set_gate(5, (uint64_t)isr5, KERNEL_CS, gate_attributes,
               0); // Bound Range Exceeded
  idt_set_gate(6, (uint64_t)isr6, KERNEL_CS, gate_attributes,
               0); // Invalid Opcode
  idt_set_gate(7, (uint64_t)isr7, KERNEL_CS, gate_attributes,
               0); // Device Not Available
  idt_set_gate(8, (uint64_t)isr8, KERNEL_CS, gate_attributes,
               0); // Double Fault (Consider using IST for this one)
  idt_set_gate(9, (uint64_t)isr9, KERNEL_CS, gate_attributes,
               0); // Coprocessor Segment Overrun
  idt_set_gate(10, (uint64_t)isr10, KERNEL_CS, gate_attributes,
               0); // Invalid TSS
  idt_set_gate(11, (uint64_t)isr11, KERNEL_CS, gate_attributes,
               0); // Segment Not Present
  idt_set_gate(12, (uint64_t)isr12, KERNEL_CS, gate_attributes,
               0); // Stack-Segment Fault
  idt_set_gate(13, (uint64_t)isr13, KERNEL_CS, gate_attributes,
               0); // General Protection Fault
  idt_set_gate(14, (uint64_t)isr14, KERNEL_CS, gate_attributes,
               0); // Page Fault
  // ISR 15 is reserved
  idt_set_gate(16, (uint64_t)isr16, KERNEL_CS, gate_attributes,
               0); // x87 Floating-Point
  idt_set_gate(17, (uint64_t)isr17, KERNEL_CS, gate_attributes,
               0); // Alignment Check
  idt_set_gate(18, (uint64_t)isr18, KERNEL_CS, gate_attributes,
               0); // Machine Check
  idt_set_gate(19, (uint64_t)isr19, KERNEL_CS, gate_attributes,
               0); // SIMD Floating-Point
  idt_set_gate(20, (uint64_t)isr20, KERNEL_CS, gate_attributes,
               0); // Virtualization

  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 0, (uint64_t)irq_stub_0, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 1, (uint64_t)irq_stub_1, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 2, (uint64_t)irq_stub_2, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 3, (uint64_t)irq_stub_3, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 4, (uint64_t)irq_stub_4, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 5, (uint64_t)irq_stub_5, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 6, (uint64_t)irq_stub_6, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_MASTER + 7, (uint64_t)irq_stub_7, KERNEL_CS,
               irq_gate_attributes, 0);

  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 0, (uint64_t)irq_stub_8, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 1, (uint64_t)irq_stub_9, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 2, (uint64_t)irq_stub_10, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 3, (uint64_t)irq_stub_11, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 4, (uint64_t)irq_stub_12, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 5, (uint64_t)irq_stub_13, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 6, (uint64_t)irq_stub_14, KERNEL_CS,
               irq_gate_attributes, 0);
  idt_set_gate(PIC_IRQ_OFFSET_SLAVE + 7, (uint64_t)irq_stub_15, KERNEL_CS,
               irq_gate_attributes, 0);

  idt_load(&idtp);
}
