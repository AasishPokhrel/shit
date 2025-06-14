#include <drivers/pic.h>
#include <shit/io.h>
#include <shit/printk.h>

#define ICW1_INIT 0x10
#define ICW1_ICW4_NEEDED 0x01
#define PIC_OCW3_READ_ISR 0x0B
#define ICW4_8086_MODE 0x01

void pic_remap_and_init(void) {
  // We need to send this both to the master and slave PICs.
  // ICW1_ICW4_NEEDED tells them to expect an ICW4 command later.
  outb(PIC1_COMMAND_PORT, ICW1_INIT | ICW1_ICW4_NEEDED);
  io_wait();
  outb(PIC2_COMMAND_PORT, ICW1_INIT | ICW1_ICW4_NEEDED);
  io_wait();

  // This is where we tell the PICs where their IRQs should start in our IDT.
  outb(PIC1_DATA_PORT, PIC_IRQ_OFFSET_MASTER);
  io_wait();
  outb(PIC2_DATA_PORT, PIC_IRQ_OFFSET_SLAVE);
  io_wait();

  // Tell Master PIC that there's a slave PIC connected to its IRQ2 line.
  // The value 4 means bit 2 is set (00000100b), indicating IRQ line 2.
  outb(PIC1_DATA_PORT, 0x04);
  io_wait();

  // Tell Slave PIC its "cascade identity" - it's the one connected to IRQ2 of
  // the master. The value 2 means it identifies itself as being on line 2.
  outb(PIC2_DATA_PORT, 0x02);
  io_wait();

  outb(PIC1_DATA_PORT, ICW4_8086_MODE);
  io_wait();
  outb(PIC2_DATA_PORT, ICW4_8086_MODE);
  io_wait();

  // Now that they're remapped, let's mask all IRQs on both PICs.
  // This means no hardware interrupts will be passed to the CPU until we
  // specifically unmask them (e.g., when a driver for that device is ready).
  outb(PIC1_DATA_PORT, 0xFF);
  outb(PIC2_DATA_PORT, 0xFF);

  printk(KERN_INFO "PICs remapped: Master (0x%x-0x%x), Slave (0x%x-0x%x)\n",
         PIC_IRQ_OFFSET_MASTER, PIC_IRQ_OFFSET_MASTER + 7, PIC_IRQ_OFFSET_SLAVE,
         PIC_IRQ_OFFSET_SLAVE + 7);
}

void pic_send_eoi(uint8_t irq_number) {
  if (irq_number >= 8 && irq_number <= 15) {
    outb(PIC2_COMMAND_PORT, PIC_EOI);
  }
  outb(PIC1_COMMAND_PORT, PIC_EOI);
}

void pic_mask_irq(uint8_t irq_line) {
  uint16_t port;
  uint8_t value;

  if (irq_line < 8) {
    port = PIC1_DATA_PORT;
  } else if (irq_line < 16) {
    port = PIC2_DATA_PORT;
    irq_line -= 8;
  } else {
    return;
  }
  value = inb(port) | (1 << irq_line);
  outb(port, value);
}

void pic_unmask_irq(uint8_t irq_line) {
  uint16_t port;
  uint8_t value;

  if (irq_line < 8) {
    port = PIC1_DATA_PORT;
  } else if (irq_line < 16) {
    port = PIC2_DATA_PORT;
    irq_line -= 8;
  } else {
    return;
  }
  value = inb(port) & ~(1 << irq_line);
  outb(port, value);
}

uint8_t pic_read_master_isr(void) {
  outb(PIC1_COMMAND_PORT, PIC_OCW3_READ_ISR);
  return inb(PIC1_COMMAND_PORT);
}

uint8_t pic_read_slave_isr(void) {
  outb(PIC2_COMMAND_PORT, PIC_OCW3_READ_ISR);
  return inb(PIC2_COMMAND_PORT);
}
