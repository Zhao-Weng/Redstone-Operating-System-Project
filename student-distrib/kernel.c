#include "multiboot.h"
#include "debug.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "paging.h"
#include "rtc.h"
#include "exception.h"
#include "terminal.h"
#include "interrupt_wrapper.h"
#include "keyboard_handler.h"
#include "exception_wrappers.h"
#include "systemcall.h"
#include "systemcall_wrapper.h"
#include "filesystem.h"
#include "schedule.h"
#include "pit.h"
#include "mousedriver.h"
#include "malloc.h"

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

#define EXCEPTION_NUM 0x20
#define RTC_ENTRY 0x28
#define KEYBRD_ENTRY 0x21
#define MOUSE_ENTRY 0x2c
#define SYSCALL_ENTRY 0X80
#define PIT_ENTRY 0x20
#define MOUSE_IRQ 0xc

extern void init_rtc();
extern void init_exception(void);

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	uint32_t file_system_start;
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;

		file_system_start = mod->mod_start;  // store mod_start as start address of file system
		
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
			mod++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000 - 4;
		ltr(KERNEL_TSS);
	}

	//initialize first 32 IDT entries for exceptions
	int i;
	for (i=0; i<EXCEPTION_NUM; i++)
	{

		idt[i].seg_selector = KERNEL_CS;    // set up elements in the exception entry
		idt[i].reserved4 = 0;			
		idt[i].reserved3 = 0;		
		idt[i].reserved2 = 1;
		idt[i].reserved1 = 1;
		idt[i].size = 1;
		idt[i].reserved0 = 0;
		idt[i].dpl = 0;          
		idt[i].present = 1;

		// set offset_31_16 and offset_15_00
		SET_IDT_ENTRY(idt[i], reserved);
	}

	//populate the IDT table with exception handlers
    init_exception();


	// set idt entry for keyboard
	idt[KEYBRD_ENTRY].seg_selector = KERNEL_CS;   // set up elements in the keyboard entry
	idt[KEYBRD_ENTRY].reserved4 = 0;			
	idt[KEYBRD_ENTRY].reserved3 = 0;		
	idt[KEYBRD_ENTRY].reserved2 = 1;
	idt[KEYBRD_ENTRY].reserved1 = 1;
	idt[KEYBRD_ENTRY].size = 1;
	idt[KEYBRD_ENTRY].reserved0 = 0;
	idt[KEYBRD_ENTRY].dpl = 0;          
	idt[KEYBRD_ENTRY].present = 1;
	//set offset_31_16 and offset_15_00 for the first 20 entries of the idt
	SET_IDT_ENTRY(idt[KEYBRD_ENTRY], keyboard_wrapper);   // set idt entry for keyboard

		// set idt entry for keyboard
	idt[MOUSE_ENTRY].seg_selector = KERNEL_CS;   // set up elements in the keyboard entry
	idt[MOUSE_ENTRY].reserved4 = 0;			
	idt[MOUSE_ENTRY].reserved3 = 0;		
	idt[MOUSE_ENTRY].reserved2 = 1;
	idt[MOUSE_ENTRY].reserved1 = 1;
	idt[MOUSE_ENTRY].size = 1;
	idt[MOUSE_ENTRY].reserved0 = 0;
	idt[MOUSE_ENTRY].dpl = 0;          
	idt[MOUSE_ENTRY].present = 1;
	//set offset_31_16 and offset_15_00 for the first 20 entries of the idt
	SET_IDT_ENTRY(idt[MOUSE_ENTRY], mouse_wrapper);   // set idt entry for keyboard

	//set offset_31_16 and offset_15_00 for the first 20 entries of the idt
	idt[RTC_ENTRY].seg_selector = KERNEL_CS;   // set up elements in the RTC entry
	idt[RTC_ENTRY].reserved4 = 0;			
	idt[RTC_ENTRY].reserved3 = 0;		
	idt[RTC_ENTRY].reserved2 = 1; //initialize the inner parts of idt of the interrupts.
	idt[RTC_ENTRY].reserved1 = 1;
	idt[RTC_ENTRY].size = 1;
	idt[RTC_ENTRY].reserved0 = 0;
	idt[RTC_ENTRY].dpl = 0;          
	idt[RTC_ENTRY].present = 1;
	//set offset_31_16 and offset_15_00 for the first 20 entries of the idt
	SET_IDT_ENTRY(idt[RTC_ENTRY], rtc_wrapper); 

	//set idt entry for system call
	idt[SYSCALL_ENTRY].seg_selector = KERNEL_CS;		
	idt[SYSCALL_ENTRY].reserved4 = 0;
	idt[SYSCALL_ENTRY].reserved3 = 0;
	idt[SYSCALL_ENTRY].reserved2 = 1;		
	idt[SYSCALL_ENTRY].reserved1 = 1;
	idt[SYSCALL_ENTRY].size = 1;
	idt[SYSCALL_ENTRY].reserved0 = 0;
	idt[SYSCALL_ENTRY].dpl = 3;
	idt[SYSCALL_ENTRY].present = 1;
	//set offset_31_16 and offset_15_00 for the first 20 entries of the idt
	SET_IDT_ENTRY(idt[SYSCALL_ENTRY], systemcall_wrapper);
	


	// set idt entry for pit
	//set offset_31_16 and offset_15_00 for the first 20 entries of the idt
	idt[PIT_ENTRY].seg_selector = KERNEL_CS;		
	idt[PIT_ENTRY].reserved4 = 0;
	idt[PIT_ENTRY].reserved3 = 0;
	idt[PIT_ENTRY].reserved2 = 1;		
	idt[PIT_ENTRY].reserved1 = 1;
	idt[PIT_ENTRY].size = 1;
	idt[PIT_ENTRY].reserved0 = 0;
	idt[PIT_ENTRY].dpl = 3;
	idt[PIT_ENTRY].present = 1;
	SET_IDT_ENTRY(idt[PIT_ENTRY], schedule_wrapper);

	// pic init
	i8259_init();   
	paging_init();    // set up paging
	rtc_initialization();     // init rtc  
	keybrd_init();	// init keyboard
	init_mouse();
	pit_initialization();		// initialize pit
	initialize_schedule();	// init schedule
	init_dynamic_memory();

	fs_init(file_system_start);   // init fs

	// process initialization
	for(i = 0; i < 6; i++){
		process_state[i] = 0;
	}
	pid = -1;


	//enable irqs
	enable_irq(8);   
	enable_irq(1);  
	enable_irq(0);
	enable_irq(MOUSE_IRQ);
	sti();

	while(1){
		execute((uint8_t *) "shell");
	}

	asm volatile(".1: hlt; jmp .1;");
}
