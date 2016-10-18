#include "lib.h"
#include "exception.h"
#include "x86_desc.h"
#include "exception_wrappers.h"
/*
*   void init_exception(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt table for exceptions
*/
void init_exception(void){
	//fill the IDT with exception handler function pointer
	SET_IDT_ENTRY(idt[0x0], divide_error_handler);
	SET_IDT_ENTRY(idt[0x1], reserved_handler);
	SET_IDT_ENTRY(idt[0x2], nmi_handler);
	SET_IDT_ENTRY(idt[0x3], breakpoint_handler);
	SET_IDT_ENTRY(idt[0x4], overflow_handler);
	SET_IDT_ENTRY(idt[0x5], bounds_handler);
	SET_IDT_ENTRY(idt[0x6], invalid_opcode_handler);
	SET_IDT_ENTRY(idt[0x7], coprocessor_not_available_handler);
	SET_IDT_ENTRY(idt[0x8], double_fault_handler);
	SET_IDT_ENTRY(idt[0x9], coprocessor_segment_overrun_handler);
	SET_IDT_ENTRY(idt[0xA], invalid_task_state_segment_handler);
	SET_IDT_ENTRY(idt[0xB], segment_not_present_handler);
	SET_IDT_ENTRY(idt[0xC], stack_fault_handler);
	SET_IDT_ENTRY(idt[0xD], general_protection_fault_handler);
	SET_IDT_ENTRY(idt[0xE], page_fault_handler);
	SET_IDT_ENTRY(idt[0xF], reserved_handler);
	SET_IDT_ENTRY(idt[0x10], math_fault_handler);
	SET_IDT_ENTRY(idt[0x11], alignment_check_handler);
	SET_IDT_ENTRY(idt[0x12], machine_check_handler);
	SET_IDT_ENTRY(idt[0x13], simd_floating_point_exception_handler);
}



/*
*   void divide_error(void)
*   Inputs: void
*   Function: handles divide error exception
*/
void divide_error(void)
{
	clear();
	printf("Divide Error", 1);
	while(1);
}

/*
*   void reserved(void)
*   Inputs: void
*   Function: handles reserved exception
*/
void reserved(void){
	clear();
	printf("RESERVED", 1);
	while(1);
}

/*
*   void nmi(void)
*   Inputs: void
*   Function: handles NMI exception
*/
void nmi(void){
	clear();
	printf("NMI Interrupt", 1);
	while(1);
}


/*
*   void breakpoint(void)
*   Inputs: void
*   Function: handles breakpoint exception
*/
void breakpoint(void){

	clear();
	printf("Overflow", 1);
	while(1);
}


/*
*   void overflow(void)
*   Inputs: void
*   Function: handles overflow exception
*/
void overflow(void){
	clear();
	printf("Breakpoint", 1);
	while(1);
}


/*
*   void bound_range_exceeded(void)
*   Inputs: void
*   Function: handles bound_range_exceeded exception
*/
void bound_range_exceeded(void){
	clear();
	printf("BOUND Range Exceeded", 1);
	while(1);
}


/*
*   void invalid_opcode(void)
*   Inputs: void
*   Function: handles invalid_opcode exception
*/
void invalid_opcode(void){
	clear();
	printf("Invalid Opcode", 1);
	while(1);
}


/*
*   void device_not_available(void)
*   Inputs: void
*   Function: handles device_not_available exception
*/
void device_not_available(void){
	clear();
	printf("Device Not Available", 1);
	while(1);
}


/*
*   void double_fault(void)
*   Inputs: void
*   Function: handles double_fault exception
*/
void double_fault(void){
	clear();
	printf("Double Fault", 1);
	while(1);
}

/*
*   void coprocessor_segment_overrun(void)
*   Inputs: void
*   Function: handles coprocessor_segment_overrun exception
*/
void coprocessor_segment_overrun(void){
	clear();
	printf("Coprocessor Segment Overrun", 1);
	while(1);
}


/*
*   void invalid_tss(void)
*   Inputs: void
*   Function: handles invalid_tss exception
*/
void invalid_tss(void){
	clear();
	printf("Invalid TSS", 1);
	while(1);

}


/*
*   void segment_not_present(void)
*   Inputs: void
*   Function: handles segment_not_present exception
*/
void segment_not_present(void){
	clear();
	printf("Segment Not Present", 1);
	while(1);

}


/*
*   void stack_segment_falut(void)
*   Inputs: void
*   Function: handles stack_segment_falut exception
*/
void stack_segment_falut(void){
	clear();
	printf("Stack-Segment Fault", 1);
	while(1);
}


/*
*   void general_protection(void)
*   Inputs: void
*   Function: handles general_protection exception
*/
void general_protection(void){
	clear();
	printf("General Protection", 1);
	while(1);
}


/*
*   void page_fault(void)
*   Inputs: void
*   Function: handles page_fault exception
*/
void page_fault(void){
	//clear();
	int fault;
	asm volatile("movl %%cr2, %0"		\
		:				\
		:"r"(fault)		\
		:"memory");
	printf("\nfault: %x\n", fault);
	printf("Page Fault Exception!!");
	while(1);
}


/*
*   void floating_point_error(void)
*   Inputs: void
*   Function: handles floating_point_error exception
*/
void floating_point_error(void){
	clear();
	printf("x87 FPU Floating-Point Error", 1);
	while(1);
}


/*
*   void alignment_check(void)
*   Inputs: void
*   Function: handles alignment_check exception
*/
void alignment_check(void){

	clear();
	printf("Alignment Check", 1);
	while(1);
}


/*
*   void machine_check(void)
*   Inputs: void
*   Function: handles machine_check exception
*/
void machine_check(void){

	clear();
	printf("Machine Check", 1);
	while(1);
}


/*
*   void simd_float_point_exception(void)
*   Inputs: void
*   Function: handles simd_float_point_exceptionexception
*/
void simd_float_point_exception(void){
	clear();
	printf("SIMD Floating-Point Exception", 1);
	while(1);

}
