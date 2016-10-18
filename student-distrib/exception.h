#ifndef _EXCEPTION_H
#define _EXCEPTION_H

/*
*   void init_exception(void)
*   Inputs: void
*   Outputs: none
*   Function: set up idt table for exceptions
*/
void init_exception(void);
/*
*   void divide_error(void);
*   Inputs: void
*   Function: handles divide error exception
*/
void divide_error(void);
/*
*   void reserved(void);
*   Inputs: void
*   Function: handles reserved exception
*/
void reserved(void);
/*
*   void nmi(void);
*   Inputs: void
*   Function: handles NMI exception
*/
void nmi(void);
/*
*   void breakpoint(void);
*   Inputs: void
*   Function: handles breakpoint exception
*/
void breakpoint(void);
/*
*   void overflow(void);
*   Inputs: void
*   Function: handles overflow exception
*/
void overflow(void);
/*
*   void bound_range_exceeded(void);
*   Inputs: void
*   Function: handles bound_range_exceeded exception
*/
void bound_range_exceeded(void);
/*
*   void invalid_opcode(void);
*   Inputs: void
*   Function: handles invalid_opcode exception
*/
void invalid_opcode(void);
/*
*   void device_not_available(void);
*   Inputs: void
*   Function: handles device_not_available exception
*/
void device_not_available(void);
/*
*   void double_fault(void);
*   Inputs: void
*   Function: handles double_fault exception
*/
void double_fault(void);
/*
*   void coprocessor_segment_overrun(void);
*   Inputs: void
*   Function: handles coprocessor_segment_overrun exception
*/
void coprocessor_segment_overrun(void);
/*
*   void invalid_tss(void);
*   Inputs: void
*   Function: handles invalid_tss exception
*/
void invalid_tss(void);
/*
*   void segment_not_present(void);
*   Inputs: void
*   Function: handles segment_not_present exception
*/
void segment_not_present(void);
/*
*   void stack_segment_falut(void);
*   Inputs: void
*   Function: handles stack_segment_falut exception
*/
void stack_segment_falut(void);
/*
*   void general_protection(void);
*   Inputs: void
*   Function: handles general_protection exception
*/
void general_protection(void);
/*
*   void page_fault(void);
*   Inputs: void
*   Function: handles page_fault exception
*/
void page_fault(void);
/*
*   void floating_point_error(void);
*   Inputs: void
*   Function: handles floating_point_error exception
*/
void floating_point_error(void);
/*
*   void alignment_check(void);
*   Inputs: void
*   Function: handles alignment_check exception
*/
void alignment_check(void);
/*
*   void machine_check(void);
*   Inputs: void
*   Function: handles machine_check exception
*/
void machine_check(void);
/*
*   void simd_float_point_exception(void);
*   Inputs: void
*   Function: handles simd_float_point_exceptionexception
*/
void simd_float_point_exception(void);

#endif
