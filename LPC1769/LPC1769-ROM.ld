/* Linker script for Cortex-M3
 *
 * Version:CodeSourcery Sourcery G++ Lite 2007q3-53
 * BugURL:https://support.codesourcery.com/GNUToolchain/
 *
 *  Copyright 2007 CodeSourcery.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply. */

/* mthomas:
    - ROM length to 256k for lpc1766
    - commented out GROUP (portablity for non-CS toolchains
    - added _sdata, _sbss, _ebss
    - modified ENTRY
    - added KEEP for vector
*/

/* Mario Ivancic, v3
	- modified for LPC1343
	- added some variables
	- modified for LPC1769
*/

/* RAM size, in kB */
__ram_size_kb = 32;

/* reserved portions of RAM for interrupt vectors, ISP and IAP
on low side:
    - we have to reserve 0x180 bytes for ROM MSD USB drivers
    - we have to reserve 0x200 (512) bytes for interrupt vectors
    - ISP uses 0x1000017C to 0x1000025B -- this part can be used but reboot will destroy it
on high side:
    - IAP uses last 32 bytes */
__ram_reserve_low0 = 0x200;     /* for vectors */
__ram_reserve_low1 = 0x260;     /* for ISP */
__ram_reserve_high = 32;

MEMORY
{
    ROM (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
    RAM (rwx) : ORIGIN = 0x10000000, LENGTH =  32K
    AHBB0(rw) : ORIGIN = 0x2007C000, LENGTH = 16K
	AHBB1(rw) : ORIGIN = 0x20080000, LENGTH = 16K
}

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(_start)
SEARCH_DIR(.)

/* These force the linker to search for particular symbols from
 * the start of the link process and thus ensure the user's
 * overrides are picked up
 */
EXTERN(_start)
EXTERN(_stack)
EXTERN(main)
EXTERN(__interrupt_vector_crc)

/* stack is on top of usable RAM */
_stack = ORIGIN(RAM) + LENGTH(RAM) - __ram_reserve_high;

/* _user_flash_start is in ROM after .data section */
/* flash granularity (sector size) */
__flash_granularity = ((_etext + SIZEOF(.data)) < 64k)?4096:32768;
_user_flash_start = (_etext + SIZEOF(.data) + __flash_granularity - 1) & ~(__flash_granularity - 1);

/* _user_flash_end is last word in ROM */
_user_flash_end = ORIGIN(ROM) + LENGTH(ROM) - 4;

/* interrupt_vector_crc is negative sum of first 7 vectors. We must add 6 because we have 6 thumb function pointers */
__interrupt_vector_crc = 0 - (6 + _stack + _start + NMI_Handler + HardFault_Handler + MemManage_Handler + BusFault_Handler + UsageFault_Handler);

SECTIONS
{
    .text :
    {
        __interrupt__vector__ = .;

        KEEP(*(.cm3.interrupt_vector .cm3.interrupt_vector.*))
        LONG (__interrupt_vector_crc)
        KEEP(*(.cm3.interrupt_vector2 .cm3.interrupt_vector2.*))

        /* Make sure we pulled in an interrupt vector.  */
        ASSERT (. != __interrupt__vector__, "No interrupt vector");

        *(.rom)
        *(.rom.b .rom.* .rom.b.*)

        __reset = .;

        *(.cm3.reset .cm3.reset.*)

        /* Make sure we pulled in some reset code.  */
        ASSERT (. != __reset, "No reset code");

        /* Serial number must be on known location 0x02dc because it's patched before flashing.
		It's 36 bytes long, 32 for serial number and 4 for code_read_protection_word @ 0x02fc */
		. = 0x02dc;
		KEEP(*(.cm3.serial_number .cm3.serial_number.*))

        *(.text .text.* .gnu.linkonce.t.*)
        *(.plt)
        *(.gnu.warning)
        *(.glue_7t) *(.glue_7) *(.vfp11_veneer)

        *(.rodata .rodata.* .gnu.linkonce.r.*)
/*
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        *(.gcc_except_table)
        *(.eh_frame_hdr)
        *(.eh_frame)
*/
        . = ALIGN(4);
        KEEP(*(.init))

        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

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


    /* skip first __ram_reserve_low0 bytes */
    .data (ORIGIN(RAM) + __ram_reserve_low0) :
    {
        _sdata = .;

        KEEP(*(.jcr))
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.* .gnu.linkonce.d.*)
        *(.ram)
        . = ALIGN (8);

        _edata = .;

    } > RAM AT > ROM

    .bss (NOLOAD) :
    {
        _sbss = .;

        *(.shbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        *(.ram.b)
        . = ALIGN (8);

        _ebss = .;

    } > RAM

    /* for noinit section we must skip ISP data area 0x1000017C to 0x1000025B */

    .noinit MAX(ORIGIN(RAM) + __ram_reserve_low1, .) (NOLOAD) :
	{
        . = ALIGN(8);
		*(.bssnoinit .bssnoinit.*)
		*(.noinit .noinit.*)
		. = ALIGN (8);
		_heap_start = .;
		_end = .;
	} > RAM

    .heap :
    {
        *(.heap .heap.*)
    } > RAM

    .ahb_bank0(NOLOAD) :
	{
		*(.ahb_sram_bank0 .ahb_sram_bank0.*)
	} > AHBB0

	.ahb_bank1(NOLOAD) :
	{
		*(.ahb_sram_bank1 .ahb_sram_bank1.*)
	} > AHBB1

	/* neinicijalizovani podaci u ramu koji se ne brisu u crt0.S */
/*	ASSERT((_end == _ebss), "ASSERT _end != _ebss")
	ASSERT((_end == _ebss) || ((_ebss >= (ORIGIN(RAM) + __ram_reserve_low1))), "BSSNOINIT sekcija mora biti iznad ISP rama @ 0x40000200")
*/
    .stab 0 (NOLOAD) : { *(.stab) }
    .stabstr 0 (NOLOAD) : { *(.stabstr) }
    /* DWARF debug sections.
    * Symbols in the DWARF debugging sections are relative to the beginning
    * of the section so we begin them at 0.  */
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

    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
    /DISCARD/ : { *(.note.GNU-stack) }
}
