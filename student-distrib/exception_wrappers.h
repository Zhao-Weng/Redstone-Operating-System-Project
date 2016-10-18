
#ifndef _EXCEPTION_WRAPPERS_H
#define _EXCEPTION_WRAPPERS_H

/*
*   void divide_error_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void divide_error_handler(void);
/*
*   void reserved_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void reserved_handler(void);
/*
*   void nmi_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void nmi_handler(void);
/*
*   void breakpoint_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void breakpoint_handler(void);
/*
*   void overflow_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void overflow_handler(void);
/*
*   void bounds_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void bounds_handler(void);
/*
*   void invalid_opcode_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void invalid_opcode_handler(void);
/*
*   void coprocessor_not_available_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void coprocessor_not_available_handler(void);
/*
*   void double_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void double_fault_handler(void);
/*
*   void coprocessor_segment_overrun_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void coprocessor_segment_overrun_handler(void);
/*
*   void invalid_task_state_segment_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void invalid_task_state_segment_handler(void);
/*
*   void segment_not_present_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void segment_not_present_handler(void);
/*
*   void stack_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void stack_fault_handler(void);
/*
*   void general_protection_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void general_protection_fault_handler(void);
/*
*   void page_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void page_fault_handler(void);
/*
*   void math_fault_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void math_fault_handler(void);
/*
*   void alignment_check_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void alignment_check_handler(void);
/*
*   void machine_check_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void machine_check_handler(void);
/*
*   void simd_floating_point_exception_handler(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt idt entry for exceptions
*/
extern void simd_floating_point_exception_handler(void);
#endif

