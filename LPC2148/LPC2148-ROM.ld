/***********************************************************************/
/*  LPC2148-ROM-S4.ld:  Linker Script File                             */
/***********************************************************************/

/* RAM_SIZE = 32k;
// RAM_END = 0x40000000 + RAM_SIZE - 32;
// ROM_SIZE = 500k; */

/* reserved portions of RAM for interrupt vectors, ISP and IAP
on low side:
    - we have to reserve 0x40 (64) bytes for interrupt vectors
    - ISP uses 0x40000120 to 0x400001ff -- this part can be used but reboot will destroy it
on high side:
    - IAP uses last 32 bytes

	__ram_reserve_low0 = 0x40;	for vectors
	__ram_reserve_low1 = 0x200; for ISP
	__ram_reserve_high = 32;
*/



/* Memory Definitions */
MEMORY
{
	ROM (rx) : ORIGIN = 0x00000000, LENGTH = 500k
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 32k
	USBRAM (rw) : ORIGIN = 0x7fd00000, LENGTH = 8k
}

ENTRY(_start)

SEARCH_DIR(.)

/* These force the linker to search for particular symbols from
 * the start of the link process and thus ensure the user's
 * overrides are picked up
 */
EXTERN(_start)
EXTERN(_stack)
EXTERN(main)

/* stack is on top of usable ram */
_stack = ORIGIN(RAM) + LENGTH(RAM) - 32;

/* _user_flash_start is in ROM after .data section */
/* flash granularity (sector size) */
__flash_granularity = ((_etext + SIZEOF(.data)) < 32k)?4096:32768;
_user_flash_start = (_etext + SIZEOF(.data) + __flash_granularity - 1) & ~(__flash_granularity - 1);

/* _user_flash_end is last word in ROM */
_user_flash_end = ORIGIN(ROM) + LENGTH(ROM) - 4;


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
		*(.rodata .rodata*)                 /* read-only data (constants) */
		*(.gnu.linkonce.r.*)

		/* .ctors .dtors are used for c++ constructors/destructors */
		/* added by Martin Thomas 4/2005 based on Anglia Design example */
		. = ALIGN(4);
		__ctors_start__ = . ;
		KEEP(*(SORT(.ctors.*)))
		KEEP(*(.ctors))
		__ctors_end__ = . ;
		. = ALIGN(4);
		__dtors_start__ = . ;
		KEEP(*(SORT(.dtors.*)))
		KEEP(*(.dtors))
		__dtors_end__ = . ;
		. = ALIGN(4);
	} > ROM = 0xffffffff
	
	/DISCARD/ :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
	__exidx_start = .;
/*
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ROM = 0xffffffff
*/
	__exidx_end = .;
	
	.text.align :
    {
        . = ALIGN(8);
        _etext = .;
    } > ROM = 0xffffffff

	/* .data section which is used for initialized data */
	/* skip first 0x40 bytes */
	.data (ORIGIN(RAM) + 0x40) :
	{
		_sdata = .;
		*(.data .data.*)
		*(.gnu.linkonce.d*)
		SORT(CONSTRUCTORS) /* mt 4/2005 */
		. = ALIGN(4);
		*(.fastrun .fastrun.*) /* !!!! "RAM-Function" example */
		*(.ramfcn .ramfcn.*) /* RAM Function */

		. = ALIGN(4);
		_edata = . ;
	} > RAM AT > ROM


	/* .bss section which is used for uninitialized data */
	.bss (NOLOAD) :
	{
		_sbss = . ;
		*(.bss .bss.*)
		*(.gnu.linkonce.b*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = . ;
	} > RAM

	/* for noinit section we must skip ISP data */
	.noinit MAX(ORIGIN(RAM) + 0x200, .) (NOLOAD) :
	{
        _sbssnoinit = . ;
        . = ALIGN(4);
		*(.bssnoinit .bssnoinit.*)
		*(.noinit .noinit.*)
		. = ALIGN (4);
		_heap_start = .;
		_end = .;
	} > RAM

	.heap :
    {
        *(.heap .heap.*)
    } > RAM

	.usbram(NOLOAD) :
	{
		*(.usbram .usbram.*)
	} > USBRAM

	/* neinicijalizovani podaci u ramu koji se ne brisu u crt0.S */
	ASSERT(((_sbssnoinit >= 0x40000200)||(_end == _ebss)), "BSSNOINIT sekcija mora biti iznad ISP rama @ 0x40000200")

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
