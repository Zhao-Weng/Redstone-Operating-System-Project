/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#define ICW1 0x11
#define ICW4 0x01
#define PIC1 0x20
#define PIC2 0x28
#define INITIAL_SLAVE_MASK 0xFF
#define INITIAL_MASTER_MASK 0xFF

//keep track of the current master and slave mask
uint8_t master_mask;
uint8_t slave_mask; 

/*
*   void i8259_init(void)
*   Inputs: void
*   Outputs: none
*   Function: send initilizaton command words to i8259
*/
void i8259_init(void)
{
	//initilialize both slave and master to mask all IRQ
	master_mask = INITIAL_MASTER_MASK;
	slave_mask = INITIAL_SLAVE_MASK;

	//send ICW1
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	//send ICW2, map PICs to corresponding vectors
	outb(PIC1, MASTER_8259_PORT_DATA);
	outb(PIC2, SLAVE_8259_PORT_DATA);	

	//send ICW3 to initialize the master slave configuration
	outb(1 << ICW3_SLAVE, MASTER_8259_PORT_DATA);
	outb(ICW3_SLAVE, SLAVE_8259_PORT_DATA);

	//send ICW4
	outb(ICW4, MASTER_8259_PORT_DATA);
	outb(ICW4, SLAVE_8259_PORT_DATA);

	//set the initial masks
	outb(master_mask, MASTER_8259_PORT_DATA);
	outb(slave_mask, SLAVE_8259_PORT_DATA);
}

/*
*   void enable_irq(int irq_num)
*   Inputs: irq_num, the IRQ number to enable
*   Outputs: none
*   Function: enable the corresponding IRQ
*/
void enable_irq(uint32_t irq_num)
{
	irq_num = irq_num + PIC1;
	const unsigned int bit_mask = 0x01;
	unsigned char out_value;
	// if enable master IRQ
	if (irq_num < PIC2){
		out_value =  (~(bit_mask << (irq_num - PIC1))) & master_mask;
		outb(out_value, MASTER_8259_PORT_DATA);
		//save updated master mask
		master_mask = out_value;
	}
	// if enable slave IRQ
	else{
		out_value = (~(bit_mask << 2)) & master_mask;
		outb(out_value, MASTER_8259_PORT_DATA);  //enable IRQ2 on the master
		master_mask = out_value; //save updated master mask
		irq_num = irq_num - PIC2;                             
		out_value = (~(bit_mask<<irq_num)) & slave_mask;  //get the mask register for slave
		outb(out_value, SLAVE_8259_PORT_DATA);   //enable slave IRQ
		slave_mask = out_value; //save updated slave mask
	}
}

/*
*   void disable_irq(int irq_num)
*   Inputs: irq_num, the IRQ number to disable
*   Outputs: none
*   Function: disable the corresponding IRQ
*/
void disable_irq(uint32_t irq_num)
{
	irq_num = irq_num + PIC1;
	const unsigned char bit_mask = 0x01;
	unsigned char out_value;
	// if disable master IRQ
	if (irq_num < PIC2){
		out_value =  ((bit_mask << (irq_num - PIC1))) | master_mask;
		outb(out_value, MASTER_8259_PORT_DATA);
		master_mask = out_value; //save the updated master mask
	}
	// if disable slave IRQ
	else{
		if (slave_mask == INITIAL_SLAVE_MASK){
			out_value = (bit_mask << 2) | master_mask;
			outb(out_value, MASTER_8259_PORT_DATA);  //disable IRQ2 on the master
			master_mask = out_value; //save the updated master mask
		}
		irq_num = irq_num - PIC2;                             
		out_value = ((bit_mask<<irq_num)) | slave_mask;  //get the mask register for slave
		outb(out_value, SLAVE_8259_PORT_DATA);   //disable slave IRQ
		slave_mask = out_value; //save the updated slave mask
	}
}

/*
*   void send_eoi(int irq_num)
*   Inputs: irq_num, the IRQ number to send EOI to
*   Outputs: none
*   Function: send EOI to device on the corresponding IRQ
*/
void send_eoi(uint32_t irq_num)
{
	irq_num = irq_num + PIC1;
	unsigned char EOI_to_slave;
	unsigned char EOI_to_master;
	//check if need to send EOI to both slave and master
	if(irq_num >= PIC2){
		//send EOI to slave
		EOI_to_slave = EOI | (irq_num - PIC2);
		outb(EOI_to_slave, SLAVE_8259_PORT);
		//send EOI to master
		EOI_to_master = EOI | ICW3_SLAVE;
		outb(EOI_to_master, MASTER_8259_PORT);
	}
	//only send EOI to master
	else{
		EOI_to_master = EOI | (irq_num - PIC1);
		outb(EOI_to_master, MASTER_8259_PORT);
	}
}

