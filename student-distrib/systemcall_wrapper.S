.text 
.global systemcall_wrapper

#system call linkage
systemcall_wrapper:
	# push all registers
	pushal 
	pushfl

	# check if irq_num is valid 
	cmpl $1,%eax
	jl invalid
	cmpl $12,%eax
	jg invalid

	pushl %edx
	pushl %ecx
	pushl %ebx

	decl %eax
	# jump based on eax, irq_num
	call *jump(,%eax,4)   
	movl %eax,save_eax

	popl %ebx
	popl %ecx
	popl %edx

	popfl 
	popal 
	movl save_eax,%eax

	sti
	iret

invalid: 
	popfl 
	popal 
	sti 
	movl $-1,%eax
	iret

# jump table
jump:
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn, malloc, free

# save irq_num
save_eax:
	.long 0

