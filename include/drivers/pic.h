#ifndef DRIVERS_PIC_H
#define DRIVERS_PIC_H

#include <stdint.h>

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT    0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT    0xA1

#define PIC_EOI 0x20

#define PIC_IRQ_OFFSET_MASTER 0x20
#define PIC_IRQ_OFFSET_SLAVE  0x28

/**
 * @brief Initializes and remaps the 8259 PICs.
 * All IRQs are initially masked after remapping.
 */
void pic_remap_and_init(void);

/**
 * @brief Sends an End-Of-Interrupt (EOI) signal to the PIC(s).
 * Must be called at the end of an IRQ handler.
 * @param irq_number The original IRQ number (0-15) that was handled.
 */
void pic_send_eoi(uint8_t irq_number);

/**
 * @brief Masks (disables) a specific IRQ line on the PIC.
 * @param irq_line The IRQ line number (0-15) to mask.
 */
void pic_mask_irq(uint8_t irq_line);

/**
 * @brief Unmasks (enables) a specific IRQ line on the PIC.
 * @param irq_line The IRQ line number (0-15) to unmask.
 */
void pic_unmask_irq(uint8_t irq_line);

/**
 * @brief Reads the In-Service Register (ISR) of the Master PIC.
 * @return The 8-bit ISR value of the Master PIC.
 */
uint8_t pic_read_master_isr(void);

/**
 * @brief Reads the In-Service Register (ISR) of the Slave PIC.
 * @return The 8-bit ISR value of the Slave PIC.
 */
uint8_t pic_read_slave_isr(void);

#endif // DRIVERS_PIC_H