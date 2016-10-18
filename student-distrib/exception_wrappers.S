.global divide_error_handler, reserved_handler, nmi_handler, breakpoint_handler, overflow_handler,bounds_handler, invalid_opcode_handler, coprocessor_not_available_handler, double_fault_handler, coprocessor_segment_overrun_handler, invalid_task_state_segment_handler, segment_not_present_handler, stack_fault_handler, general_protection_fault_handler, page_fault_handler, math_fault_handler, alignment_check_handler, machine_check_handler, simd_floating_point_exception_handler

/*
*   void divide_error_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
divide_error_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call divide_error
	popfl    # pop all flags 
	popal    # pop all registers	
	iret 
/*
*   void reserved_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
reserved_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call reserved
	popfl    # pop all flags 
	popal    # pop all registers
	
	iret
/*
*   void nmi_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
nmi_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call nmi
	popfl    # pop all flags 
	popal    # pop all registers
	iret

/*
*   void breakpoint_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
breakpoint_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call breakpoint 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret

/*
*   void overflow_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
overflow_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call overflow
	popfl    # pop all flags 
	popal    # pop all registers	
	iret
/*
*   void bounds_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
bounds_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call bound_range_exceeded
	popfl    # pop all flags 
	popal    # pop all registers
	
	iret

/*
*   void invalid_opcode_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
invalid_opcode_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call invalid_opcode 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void coprocessor_not_available_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
coprocessor_not_available_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call device_not_available 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void double_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
double_fault_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call double_fault
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void coprocessor_segment_overrun_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
coprocessor_segment_overrun_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call coprocessor_segment_overrun
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void invalid_task_state_segment_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
invalid_task_state_segment_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call invalid_tss 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret

/*
*   void segment_not_present_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
segment_not_present_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call segment_not_present
	popfl    # pop all flags 
	popal    # pop all registers	
	iret

/*
*   void stack_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
stack_fault_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call stack_segment_falut 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret

/*
*   void general_protection_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
general_protection_fault_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call general_protection 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void page_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
page_fault_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call page_fault
	popfl    # pop all flags 
	popal    # pop all registers	
	iret

/*
*   void math_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
math_fault_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call floating_point_error 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void alignment_check_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
alignment_check_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call alignment_check 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void machine_check_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
machine_check_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call machine_check 
	popfl    # pop all flags 
	popal    # pop all registers	
	iret


/*
*   void simd_floating_point_exception_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
simd_floating_point_exception_handler:
	pushal   # push all registers
	pushfl   # push all flags 
	call simd_float_point_exception
	popfl    # pop all flags 
	popal    # pop all registers	
	iret

