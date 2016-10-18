.text # The text section is used for keeping the actual code

.global rtc_wrapper, keyboard_wrapper, schedule_wrapper, mouse_wrapper

# saves all registers and flags before rtc_handler, restores all registers and flags after
rtc_wrapper:
	pushal   # push all registers
	pushfl   # push all flags 
	call rtc_handler   
	popfl    # pop all flags 
	popal    # pop all registers
	
	iret


# saves all registers and flags before terminal, restores all registers and flags after
keyboard_wrapper:
	cli
	pushal   # push all registers
	pushfl   # push all flags 
	call keyboard_handler
	popfl    # pop all flags 
	popal    # pop all registers
	sti
	
	iret

schedule_wrapper:
	cli
	pushal
	pushfl
	call schedule_handler
	popfl
	popal
	sti

	iret

mouse_wrapper:
	cli
	pushal
	pushfl
	call mouse_handler
	popfl
	popal
	sti

	iret



