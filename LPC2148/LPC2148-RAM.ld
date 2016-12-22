/***********************************************************************/
/*  LPC2148-ROM-S4.ld:  Linker Script File                             */
/***********************************************************************/
ENTRY(_boot)
/* STACK_SIZE = 0x400; */
RAM_END = 0x40000000 + 32k - 32;
ROM_SIZE = 500k;

/* Memory Definitions */
MEMORY
{
/*	ROM (rx) : ORIGIN = 0x00000000, LENGTH = 512k
	RAM (rw) : ORIGIN = 0x40000000, LENGTH = 32k 
*/
/*
	ROM (rx) : ORIGIN = 0x00000000, LENGTH = 32k
	FLASH (r) : ORIGIN = 0x00080000, LENGTH = 468k
	RAM (rw) : ORIGIN = 0x40000000, LENGTH = 32512
*/
	ROM (rx) : ORIGIN = 0x00000000, LENGTH = 500k
	VECTORS (rw) : ORIGIN = 0x40000000, LENGTH = 64
	RAM (rw) : ORIGIN = 0x40000000 + 64, LENGTH = 32k - 32 - 64
	IAPRAM (r) : ORIGIN = 0x40000000 + 32k - 32, LENGTH = 32
	USBRAM (rw) : ORIGIN = 0x7fd0000, LENGTH = 8k
}

/* Section Definitions */
SECTIONS
{
	/* first section is .text which is used for code */
	.text :
	{
		KEEP(*(.init))             /* Startup code from .init-section, must use KEEP */
		*(.text .text.*)           /* remaining code */
		*(.gnu.linkonce.t.*)
		*(.glue_7)
		*(.glue_7t)
/*
		*(.gcc_except_table)
*/
		*(.rodata)                 /* read-only data (constants) */
		*(.rodata*)
		*(.gnu.linkonce.r.*)
	} > RAM = 0xffffffff

	. = ALIGN(4);

	/* .ctors .dtors are used for c++ constructors/destructors */
	/* added by Martin Thomas 4/2005 based on Anglia Design example */
	.ctors :
	{
		PROVIDE(__ctors_start__ = .);
		KEEP(*(SORT(.ctors.*)))
		KEEP(*(.ctors))
		PROVIDE(__ctors_end__ = .);
	} > RAM = 0xffffffff
	
	.dtors :
	{
		PROVIDE(__dtors_start__ = .); 
		KEEP(*(SORT(.dtors.*)))
		KEEP(*(.dtors))
		PROVIDE(__dtors_end__ = .);
	} > RAM = 0xffffffff
  
    . = ALIGN(4);
	/* mthomas - end */
   
	_etext = . ;
	PROVIDE (etext = .);

/*
	.flash ALIGN(32k) :
	{
		_user_flash_start = .;
		PROVIDE(user_flash_start = .);
		*(.flash)
	} > ROM = 0xffffffff
*/

	/DISCARD/ :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }
	
	/* .data section which is used for initialized data */
	.data : AT (_etext)
	{
		_data = .;
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		SORT(CONSTRUCTORS) /* mt 4/2005 */
		. = ALIGN(4);
		*(.fastrun) /* !!!! "RAM-Function" example */
		*(.ramfcn) /* RAM Function */
	} > RAM
 
	. = ALIGN(4);
	_edata = . ;
	PROVIDE (edata = .);
 

	/* .bss section which is used for uninitialized data */
	.bss (NOLOAD) :
	{
		__bss_start = . ;
		__bss_start__ = . ;
		*(.bss)
		*(.gnu.linkonce.b*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = . ;
		PROVIDE (__bss_end = .);
		/* neinicijalizovani podaci u ramu koji se ne brisu u crt0.S */
		*(.bssnoinit)
	} > RAM

	. = ALIGN(4);
	__bssnoinit_end__ = . ;
	PROVIDE (__bssnoinit_end = .);
	PROVIDE (__heap_start = .);


	.stack /*ALIGN(256)*/ :
	{
/*		. += STACK_SIZE; */
/* Stack je na kraju rama (ostavljeno 32 bajta na kraju za IAP) */
 		. += (0x40000000 + 32k - 32 - __bssnoinit_end__);
		PROVIDE (_stack = .);
	} > RAM

	_end = . ;
	PROVIDE (end = .);
  
	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
	/* DWARF debug sections.
	   Symbols in the DWARF debugging sections are relative to the beginning
	   of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
}
