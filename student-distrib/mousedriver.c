#include "mousedriver.h"


pos_t mouse;
// statue register has to be set before read data
uint8_t read_mouse() {
	while((inb(STATUS_REGISTER)&0x1)==0)
		;
	return inb(MOUSE_PORT);
}

void move (int32_t dx, int32_t dy)
{
	mouse.x += dx/2;
	if(mouse.x <= 0)
		mouse.x = 0;
	if(mouse.x >= 79)
		mouse.x = 79;
	mouse.y -= dy/2;
	if(mouse.y <= 0)
		mouse.y = 0;
	if(mouse.y >= 24)
		mouse.y = 24;
	update_cursor(mouse.y,mouse.x);


}

//status register must be cleared before write data
/*Sending a command or data byte to the mouse (to port 0x60) must be preceded by
sending a 0xD4 byte to port 0x64 (with appropriate waits on port 0x64, bit 1, 
before sending each output byte). Note: this 0xD4 byte does not generate any
ACK, from either the keyboard or mouse.*/
void write_mouse(uint8_t data, uint8_t port){
	while((inb(STATUS_REGISTER)& 0x2) !=0)
		;
	//need to write D4 before command
	outb(0xD4, STATUS_REGISTER);

	while((inb(STATUS_REGISTER)& 0x2) !=0)
		;
	outb(data, port);
	//printf("!!!!!!\n");
}


extern void mouse_handler()
{
	
	//cli();
	uint8_t package;
	if ((inb(STATUS_REGISTER) & 0x1) ==0)
		package = 0;
	else
		package = inb(MOUSE_PORT);

	if(package != 0)
	{	
		if(package == 0xFA) //ack
			;
		else
		{
			 if ((package&MOVEMENT_ONE)!=0 && (package&X_OVERFLOW)==0 && (package&Y_OVERFLOW)==0)
			{
				int32_t dx = read_mouse();
				if (package & X_SIGN)
					dx |= 0xFFFFFF00;
				int32_t dy = read_mouse();
				if (package & Y_SIGN)
					dy |= 0xFFFFFF00;  
				move(dx, dy);
			}
		}

	}
	send_eoi(MOUSE_IRQ);
	//sti();
	return;
}

void init_mouse()
{
	write_mouse(0xFF, MOUSE_PORT);//reset the mouse
	write_mouse(0x20, STATUS_REGISTER);//send "Get Compaq Status Byte" command

	uint8_t compaq_status  = read_mouse();

	compaq_status |= 0x2; //enable IRQ12
	compaq_status &= 0xDF; //clear disable mouse click
	

	write_mouse(0x60, STATUS_REGISTER);//set compaq status
	while ((inb(STATUS_REGISTER) & 0x2) !=0)
		;

	//write compaq status byte back
	outb(compaq_status, MOUSE_PORT);

	mouse.x = 0;
	mouse.y = 0;
	//enable package streaming

	write_mouse(0xF4, MOUSE_PORT);
	write_mouse(0xF3, MOUSE_PORT);
	read_mouse(); //ack
	write_mouse(200, MOUSE_PORT);

	write_mouse(0xE8, MOUSE_PORT);
	read_mouse();
	//write_mouse(0x03, MOUSE_PORT);
	//printf("stuck1\n");

}



