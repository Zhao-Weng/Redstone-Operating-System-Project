#ifndef _I8259_H
#define _I8259_H

#include "types.h"

#define MASTER_8259_PORT 0x20
#define MASTER_8259_PORT_DATA 0x21
#define SLAVE_8259_PORT  0xA0
#define SLAVE_8259_PORT_DATA 0xA1
#define ICW1    0x11
#define ICW2_MASTER   0x20
#define ICW2_SLAVE    0x28
#define ICW3_MASTER   0x04
#define ICW3_SLAVE    0x02
#define ICW4          0x01
#define EOI             0x60


/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
