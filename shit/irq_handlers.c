#include <drivers/pic.h>
#include <shit/interrupt.h>
#include <shit/printk.h>
#include <stddef.h>

// This is our array of function pointers. Think of it as a dispatch table.
// When an IRQ (0-15) comes in, we'll look up the corresponding function
// in this array and call it.
// We start with all entries as NULL, meaning no handler is registered yet.
static irq_c_handler_t s_irq_c_routines[16] = {0};

void interrupt_register_irq_handler(uint8_t irq_line, irq_c_handler_t handler) {
  if (irq_line < 16) {
    printk(KERN_DEBUG "IRQ: Registering handler for IRQ line %u at %p\n",
           irq_line, handler);
    s_irq_c_routines[irq_line] = handler;
  } else {
    printk(KERN_WARN
           "IRQ: Attempted to register handler for invalid IRQ line %u\n",
           irq_line);
  }
}

void interrupt_unregister_irq_handler(uint8_t irq_line) {
  if (irq_line < 16) {
    printk(KERN_DEBUG "IRQ: Unregistering handler for IRQ line %u\n", irq_line);
    s_irq_c_routines[irq_line] = NULL;
  }
}

void irq_c_dispatcher(irq_context_t *frame) {
  uint8_t original_irq_line = 0xFF;

  // Convert the IDT vector number (which is what the CPU gives us)
  // back to the original IRQ line number (0-15) that the PIC uses.
  if (frame->vector_number >= PIC_IRQ_OFFSET_MASTER &&
      frame->vector_number < (PIC_IRQ_OFFSET_MASTER + 8)) {
    // IRQ came from the master PIC (IRQ 0-8)
    original_irq_line = frame->vector_number - PIC_IRQ_OFFSET_MASTER;
  } else if (frame->vector_number >= PIC_IRQ_OFFSET_SLAVE &&
             frame->vector_number < (PIC_IRQ_OFFSET_SLAVE + 8)) {
    // IRQ came from the slave PIC (IRQ 8-15)
    original_irq_line = (frame->vector_number - PIC_IRQ_OFFSET_SLAVE) + 8;
  }

  if (original_irq_line == 0xFF) {
    printk(KERN_ERR "IRQ: Received interrupt with unmappable vector %lu!\n",
           frame->vector_number);
    // No EOI needed as we don't know which PIC, or if it's even a PIC IRQ.
    return;
  }

  if (original_irq_line == 7) { // Potentially spurious IRQ from Master PIC
    uint8_t master_isr = pic_read_master_isr();
    if (!(master_isr & (1 << 7))) {
      printk(KERN_WARN "IRQ: Spurious IRQ7 detected (vector %lu).\n",
             frame->vector_number);
      return;
    }
  } else if (original_irq_line == 15) {
    uint8_t slave_isr = pic_read_slave_isr();

    if (!(slave_isr & (1 << 7))) {
      printk(KERN_WARN "IRQ: Spurious IRQ15 detected (vector %lu).\n",
             frame->vector_number);

      pic_send_eoi(2);
      return;
    }
  }

  if (s_irq_c_routines[original_irq_line] != NULL) {
    s_irq_c_routines[original_irq_line](frame);
  } else {
    printk(KERN_WARN "IRQ: Unhandled IRQ line %u (IDT vector %lu)\n",
           original_irq_line, frame->vector_number);
  }

  // Important! Tell the PIC that we've finished handling this interrupt.
  // If we don't do this, the PIC might not send any more interrupts from this
  // line or lower-priority lines.
  if (original_irq_line != 0xFF) {
    pic_send_eoi(original_irq_line);
  }
}